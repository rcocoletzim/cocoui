# Cocoui Font Generator

This tool generates C++ zero-allocation font arrays for the `cocoui` framework from standard TrueType (TTF/TTC) files. It extracts glyph metrics and bitmap data to be stored directly in ROM, bypassing the need for dynamic memory allocation during rendering.

## Setup

**macOS / Linux:**
Before running the script for the first time, grant execution permissions to the shell script:
chmod +x run_font_gen.sh

**Windows:**
No special permissions are needed. Run the `.bat` file directly from your command prompt or terminal.

## Usage

The shell (`.sh`) and batch (`.bat`) scripts automatically manage the Python virtual environment, install the required dependencies (`freetype-py`), and pass any provided arguments directly to the underlying Python generator.

### Syntax

**macOS / Linux:**
./run_font_gen.sh <ttf_path> <size> [--style STYLE] [--bpp BPP] [--out OUT]

**Windows:**
run_font_gen.bat <ttf_path> <size> [--style STYLE] [--bpp BPP] [--out OUT]

### Arguments

*   `ttf_path` **(Required)**: Path to the source `.ttf` or `.ttc` file.
*   `size` **(Required)**: Font size in pixels.
*   `--style`: Font style transformation. Options: `normal` (default), `bold`, `italic`, `bold_italic`. *(Note: Using actual bold font files is recommended over synthetic bold).*
*   `--bpp`: Bits per pixel for the rendering target. Options: `8` (default, antialiased grayscale) or `1` (monochrome, packed bits).
*   `--out`: Output filename prefix. If omitted, the script auto-generates a name based on the input file, size, style, and bpp (e.g., `font_helvetica_normal_16px_8bpp`).

### Examples

**1. Basic 16px font (8bpp, antialiased):**
./run_font_gen.sh /System/Library/Fonts/Helvetica.ttc 16

**2. Packed 1bpp monochrome font (maximizes ROM savings):**
./run_font_gen.sh /System/Library/Fonts/Helvetica.ttc 14 --bpp 1

**3. Synthetic italic transformation with custom output name:**
./run_font_gen.sh /System/Library/Fonts/Helvetica.ttc 18 --style italic --out custom_italic_font

### Integration Notes
When generating fonts with `--bpp 1`, the resulting `pixel_data` array contains 8 pixels packed into a single byte. Your rendering routine in `cocoui` must use bitwise operations to extract the specific bits rather than reading full bytes per pixel.