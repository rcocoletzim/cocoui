/*
 * CocoUI Framework (https://github.com/rcocoletzim/cocoui)
 * Copyright (c) 2026 Rubén Cocoletzi Mata
 *
 * This software is provided 'as-is', under the terms of the zlib/libpng license.
 * See the LICENSE file in the project root for full license information.
 * SPDX-License-Identifier: Zlib
 */

#pragma once

#include <tuple>
#include <utility>

#include "cocoui/primitives.hpp"
#include "cocoui/widget.hpp"
#include "cocoui/events.hpp"

namespace cocoui {

template <typename... Children>
class Row : public Widget<Row<Children...>> {
   private:
    // The entire UI subtree is stored completely flat in memory (Stack/Flash)
    std::tuple<Children...> children_;

    // Compile-time recursion helper for C++14 using the "Dummy Array Trick"
    template <typename FB, std::size_t... Indices>
    void draw_impl(FB& fb, const Rect& parent_bounds, std::index_sequence<Indices...>) const {
        using expander = int[];

        // Calculate absolute position of this row based on its parent
        int16_t abs_x = parent_bounds.origin.x + bounds_.origin.x;
        int16_t abs_y = parent_bounds.origin.y + bounds_.origin.y;

        // If no explicit height is set, fill the parent's height
        int16_t h = bounds_.size.height > 0 ? bounds_.size.height : parent_bounds.size.height;

        // Unpack tuple and draw each child.
        // We advance on the X axis. Hardcoded to 40px width per child for initial testing.
        (void)expander{0,
                       (std::get<Indices>(children_).draw(
                            fb, Rect(abs_x + (static_cast<int16_t>(Indices) * 40), abs_y, 40, h)),
                        0)...};
    }

    template <std::size_t... Indices>
    bool handle_touch_impl(Point p, const Rect& parent_bounds, std::index_sequence<Indices...>) {
        int16_t abs_x = parent_bounds.origin.x + bounds_.origin.x;
        int16_t abs_y = parent_bounds.origin.y + bounds_.origin.y;
        int16_t w = bounds_.size.width > 0 ? bounds_.size.width : parent_bounds.size.width;

        bool handled = false;
        using expander = int[];

       // We now use dispatch_touch! If the child has no handle_touch, it safely returns false at compile-time.
        (void)expander{
            0, (handled ? 0
                        : (handled = cocoui::dispatch_touch(std::get<Indices>(children_),
                               p, Rect(canvas_abs_x, canvas_abs_y, canvas_w, canvas_h)),
                           0))...};
        return handled;
    }

   public:
    // Constructor moves the children into the tuple
    constexpr explicit Row(Children... children) : children_(std::move(children)...) {}

    // Public draw interface (Static Polymorphism)
    template <typename FB>
    void draw(FB& fb, const Rect& parent_bounds) const {
        if (!is_visible_) return;
        // Generate a compile-time sequence to iterate the tuple
        draw_impl(fb, parent_bounds, std::index_sequence_for<Children...>{});
    }
};

// Factory function to deduce types automatically in C++14
template <typename... Children>
constexpr Row<Children...> make_row(Children... children) {
    return Row<Children...>(std::move(children)...);
}

}  // namespace cocoui