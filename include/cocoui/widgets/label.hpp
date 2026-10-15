/*
- CocoUI Framework
- SPDX-License-Identifier: Zlib
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

        for (const char* c = text_; *c != '\0'; ++c) {
            // Conversión básica a codepoint (Asume ASCII directo por el momento)
            uint32_t codepoint = static_cast<uint32_t>(*c);
            
            const Glyph* glyph = font_->get_glyph(codepoint);
            if (glyph != nullptr) {
                const uint8_t* glyph_ptr = font_->pixel_data + glyph->data_offset;

                // Ajustar la posición sumando los offsets individuales del glifo
                int16_t draw_x = cursor_x + glyph->offset_x;
                int16_t draw_y = cursor_y + glyph->offset_y;

                fb.draw_glyph(Point(draw_x, draw_y), glyph_ptr, 
                              glyph->width, glyph->height, font_->bpp,
                              ColorConverter<P>::convert(color_));

                // Avanzar el cursor según el ancho real de la letra
                cursor_x += glyph->advance;
            }
        }
    }
};

constexpr Label make_label(const char* text, const Font& font, Color c) {
    return Label(text, &font, c);
}

}  // namespace cocoui