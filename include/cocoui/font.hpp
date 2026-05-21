/*
 * CocoUI Framework
 * SPDX-License-Identifier: Zlib
 */

#pragma once
#include <cstdint>

namespace cocoui {

/**
 * @brief Represents a monospaced 1-bit bitmap font living in Flash/ROM memory.
 */
struct Font {
    const uint8_t* bitmap;   // Pointer to the raw 1-bit array
    uint8_t glyph_width;     // Width of each character in pixels
    uint8_t glyph_height;    // Height of each character in pixels
    char first_char;         // First ASCII character available (e.g., ' ')
    char last_char;          // Last ASCII character available (e.g., '~')
};

} // namespace cocoui