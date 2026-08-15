/*
 * CocoUI Framework
 * SPDX-License-Identifier: Zlib
 */

#pragma once

#include "cocoui/font.hpp"
#include "cocoui/primitives.hpp"
#include "cocoui/widget.hpp"

namespace cocoui {

template <int16_t W, int16_t H, typename P>
class Framebuffer;

class Label : public Widget<Label> {
   private:
    const char* text_;
    const Font* font_;
    Color color_;

   public:
    constexpr explicit Label(const char* text, const Font* font, Color c)
        : text_(text), font_(font), color_(c) {}

    template <int16_t W, int16_t H, typename P>
    void draw(Framebuffer<W, H, P>& fb, const Rect& parent_bounds) const {
        if (!this->is_visible_ || !text_ || !font_) return;

        int16_t cursor_x = parent_bounds.origin.x + this->bounds_.origin.x;
        int16_t cursor_y = parent_bounds.origin.y + this->bounds_.origin.y;

        // Iterate through the string until the null-terminator '\0'
        for (const char* c = text_; *c != '\0'; ++c) {
            
            // Check if the character exists in our Font ROM
            if (*c >= font_->first_char && *c <= font_->last_char) {
                // Calculate the memory offset for this specific letter
                uint16_t char_index = *c - font_->first_char;
                // Since our font is monospaced, the offset is predictable
                const uint8_t* glyph_ptr = font_->bitmap + (char_index * font_->glyph_height);

                fb.draw_glyph(Point(cursor_x, cursor_y), glyph_ptr, 
                              font_->glyph_width, font_->glyph_height, 
                                ColorConverter<P>::convert(color_));
            }

            // Advance the cursor for the next letter
            cursor_x += font_->glyph_width;
        }
    }
};

constexpr Label make_label(const char* text, const Font& font, Color c) {
    return Label(text, &font, c);
}

}  // namespace cocoui