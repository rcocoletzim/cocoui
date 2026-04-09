/*
 * CocoUI Framework
 * SPDX-License-Identifier: Zlib
 */
#pragma once

#include "cocoui/primitives.hpp"
#include "cocoui/widget.hpp"

namespace cocoui {

template <int16_t W, int16_t H, typename P>
class Framebuffer;

// A lightweight structure to wrap a C-Array asset (intended to live in Flash/ROM)
struct Bitmap {
    Size size;
    const uint32_t* data;
};

class Image : public Widget<Image> {
   private:
    Bitmap bitmap_;

   public:
    constexpr explicit Image(Bitmap bmp) : bitmap_(bmp) {
        // By default, the widget bounds match the exact dimensions of the image asset
        this->bounds_.size = bmp.size;
    }

    template <int16_t W, int16_t H, typename P>
    void draw(Framebuffer<W, H, P>& fb, const Rect& parent_bounds) const {
        if (!this->is_visible_) return;

        // Calculate absolute position on the screen
        int16_t abs_x = parent_bounds.origin.x + this->bounds_.origin.x;
        int16_t abs_y = parent_bounds.origin.y + this->bounds_.origin.y;

        fb.draw_image(Point(abs_x, abs_y), bitmap_.data, bitmap_.size);
    }

    // A static image does not consume touch events
    bool handle_touch(Point /*p*/, const Rect& /*parent_bounds*/) {
        return false;
    }
};

// Factory helper for automatic type deduction
constexpr Image make_image(Bitmap bmp) {
    return Image(bmp);
}

}  // namespace cocoui