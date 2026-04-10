/*
 * CocoUI Framework (https://github.com/rcocoletzim/cocoui)
 * Copyright (c) 2026 Rubén Cocoletzi Mata
 *
 * This software is provided 'as-is', under the terms of the zlib/libpng license.
 * See the LICENSE file in the project root for full license information.
 * SPDX-License-Identifier: Zlib
 */
#pragma once

#include "cocoui/primitives.hpp"
#include "cocoui/widget.hpp"

// Forward declaration of a generic drawing context (we'll implement this later)
namespace cocoui {

template <int16_t W, int16_t H, typename P>
class Framebuffer;

// A primitive widget that simply fills its assigned area with a color
class SolidColor : public Widget<SolidColor> {
   private:
    Color color_;

   public:
    // constexpr ensures this widget can be constructed at compile-time
    constexpr explicit SolidColor(Color c) : color_(c) {}

    constexpr SolidColor& with_color(Color c) noexcept {
        color_ = c;
        return *this;
    }

    // Static polymorphism draw method
    template <int16_t W, int16_t H, typename P>
    void draw(Framebuffer<W, H, P>& fb, const Rect& parent_bounds) const {
        if (!is_visible_) return;

        // Compute absolute position (Parent origin + Local origin)
        Rect absolute_bounds(
            parent_bounds.origin.x + bounds_.origin.x, parent_bounds.origin.y + bounds_.origin.y,
            // If the widget has no explicit size, it fills the parent's size
            bounds_.size.width > 0 ? bounds_.size.width : parent_bounds.size.width,
            bounds_.size.height > 0 ? bounds_.size.height : parent_bounds.size.height);
        // Render to the framebuffer
        // For the simulator, to_hex() matches ARGB8888. We will abstract this later for RGB565.

        fb.fill_rect(absolute_bounds, color_.to_hex());
    }

    
};

// Factory helper for C++14
constexpr SolidColor make_solid_color(Color c) {
    return SolidColor(c);
}



}  // namespace cocoui
