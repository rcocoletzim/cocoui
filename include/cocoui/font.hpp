#pragma once

#include <cstddef>
#include <cstdint>

namespace cocoui {

struct Glyph {
    uint32_t codepoint;
    uint8_t width;
    uint8_t height;
    int8_t offset_x;
    int8_t offset_y;
    uint8_t advance;
    uint32_t data_offset;
};

struct Font {
    const Glyph* glyphs;
    size_t glyph_count;
    const uint8_t* pixel_data;
    uint8_t line_height;
    uint8_t bpp;

    constexpr const Glyph* get_glyph(uint32_t codepoint) const {
        int low = 0;
        int high = static_cast<int>(glyph_count) - 1;
        
        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (glyphs[mid].codepoint == codepoint) {
                return &glyphs[mid];
            } else if (glyphs[mid].codepoint < codepoint) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
        return nullptr;
    }
};

}  // namespace cocoui