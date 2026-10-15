import freetype
import sys
import argparse
import os

def generate_font_cpp(ttf_path, font_size, style, bpp, chars, output_name):
    face = freetype.Face(ttf_path)
    face.set_pixel_sizes(0, font_size)

    # Apply synthetic italic (shear transform) if requested
    if style in ['italic', 'bold_italic']:
        matrix = freetype.Matrix(int(1.0 * 0x10000), int(0.3 * 0x10000),
                                 int(0.0 * 0x10000), int(1.0 * 0x10000))
        face.set_transform(matrix, freetype.Vector(0, 0))

    # Note: For bold, it's highly recommended to provide the actual Bold TTF file 
    # (e.g., Helvetica-Bold.ttf) rather than generating synthetic bold, 
    # as synthetic bold outlines can distort small pixel sizes.

    bitmap_data = []
    glyphs = []
    current_offset = 0

    # Sort characters by codepoint to allow binary search in C++
    sorted_chars = sorted(list(set(chars)))

    # Determine FreeType render target based on bits per pixel
    load_flags = freetype.FT_LOAD_RENDER
    if bpp == 1:
        load_flags |= freetype.FT_LOAD_TARGET_MONO
    else:
        load_flags |= freetype.FT_LOAD_TARGET_NORMAL

    for char in sorted_chars:
        face.load_char(char, load_flags)
        bitmap = face.glyph.bitmap
        
        width = bitmap.width
        height = bitmap.rows
        pitch = bitmap.pitch
        pixels = bitmap.buffer
        
        packed_data = []
        bytes_per_row = (width * bpp + 7) // 8
        for row in range(height):
            start = row * pitch
            packed_data.extend(pixels[start : start + bytes_per_row])
            
        bitmap_data.extend(packed_data)
        
        glyphs.append({
            'codepoint': ord(char),
            'width': width,
            'height': height,
            'offset_x': face.glyph.bitmap_left,
            'offset_y': -face.glyph.bitmap_top,
            'advance': face.glyph.advance.x >> 6,
            'data_offset': current_offset
        })
        
        current_offset += len(pixels)

    # Generate C++ code
    cpp_code = f'#include "cocoui/font.hpp"\n\n'
    cpp_code += f'// Automatically generated font: {ttf_path}\n'
    cpp_code += f'// Size: {font_size}px, Style: {style}, BPP: {bpp}\n\n'
    
    # Pixel array
    cpp_code += f'static const uint8_t {output_name}_pixels[] = {{\n    '
    for i, b in enumerate(bitmap_data):
        cpp_code += f'0x{b:02X}, '
        if (i + 1) % 16 == 0:
            cpp_code += '\n    '
    cpp_code += '\n};\n\n'

    # Glyph array
    cpp_code += f'static const cocoui::Glyph {output_name}_glyphs[] = {{\n'
    for g in glyphs:
        cpp_code += f"    {{ {g['codepoint']}, {g['width']}, {g['height']}, {g['offset_x']}, {g['offset_y']}, {g['advance']}, {g['data_offset']} }},\n"
    cpp_code += '};\n\n'

    # Font definition
    line_height = face.size.height >> 6
    cpp_code += f'extern const cocoui::Font {output_name} = {{\n'
    cpp_code += f'    {output_name}_glyphs,\n'
    cpp_code += f'    sizeof({output_name}_glyphs) / sizeof(cocoui::Glyph),\n'
    cpp_code += f'    {output_name}_pixels,\n'
    cpp_code += f'    {line_height}, // line_height\n'
    cpp_code += f'    {bpp} // bpp\n'
    cpp_code += '};\n'

    with open(f'{output_name}.cpp', 'w') as f:
        f.write(cpp_code)
    
    print(f"Generated {output_name}.cpp with {len(glyphs)} glyphs and {len(bitmap_data)} bytes of bitmaps.")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Generate C++ font structures for cocoui.")
    parser.add_argument("ttf_path", type=str, help="Path to the TTF file")
    parser.add_argument("size", type=int, help="Font size in pixels")
    parser.add_argument("--style", type=str, choices=['normal', 'bold', 'italic', 'bold_italic'], default='normal', help="Font style")
    parser.add_argument("--bpp", type=int, choices=[1, 8], default=8, help="Bits per pixel (1 for mono, 8 for antialiasing)")
    parser.add_argument("--out", type=str, help="Output name prefix (e.g., font_helvetica_16)")

    args = parser.parse_args()

    # Default character set
    required_chars = "".join(chr(i) for i in range(32, 127)) + "StartStopáéíóúÁÉÍÓÚñÑüÜ¿¡"
    
    # Auto-generate output name if not provided
    if args.out:
        output_name = args.out
    else:
        base_name = os.path.splitext(os.path.basename(args.ttf_path))[0].lower().replace('-', '_').replace(' ', '_')
        output_name = f"font_{base_name}_{args.style}_{args.size}px_{args.bpp}bpp"

    generate_font_cpp(args.ttf_path, args.size, args.style, args.bpp, required_chars, output_name)