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
class Column : public Widget<Column<Children...>> {  // <--- Herencia pública para ganar la API Fluida
   private:
    // The entire UI subtree is stored completely flat in memory
    std::tuple<Children...> children_;

    // Compile-time recursion helper for C++14 using the "Dummy Array Trick"
    template <typename FB, std::size_t... Indices>
    void draw_impl(FB& fb, const Rect& parent_bounds, std::index_sequence<Indices...>) const {
        using expander = int[];

        // Calculate absolute position of this column based on its parent
        int16_t abs_x = parent_bounds.origin.x + bounds_.origin.x;
        int16_t abs_y = parent_bounds.origin.y + bounds_.origin.y;
        int16_t w = bounds_.size.width > 0 ? bounds_.size.width : parent_bounds.size.width;

        // Unpack tuple and draw each child.
        // For now, we hardcode a 40px height per child for testing.
        (void)expander{0,
                       (std::get<Indices>(children_).draw(
                            fb, Rect(abs_x, abs_y + (static_cast<int16_t>(Indices) * 40), w, 40)),
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
    constexpr explicit Column(Children... children) : children_(std::move(children)...) {}

    // Public draw interface (Static Polymorphism)
    template <typename FB>
    void draw(FB& fb, const Rect& parent_bounds) const {
        if (!is_visible_) return;
        // Generate a compile-time sequence (0, 1, 2, ..., N-1) to iterate the tuple
        draw_impl(fb, parent_bounds, std::index_sequence_for<Children...>{});
    }

    bool handle_touch(Point p, const Rect& parent_bounds) {
        if (!is_visible_) return false;
        return handle_touch_impl(p, parent_bounds, std::index_sequence_for<Children...>{});
    }
};

// Factory function to deduce types automatically in C++14
template <typename... Children>
constexpr Column<Children...> make_column(Children... children) {
    return Column<Children...>(std::move(children)...);
}

}  // namespace cocoui
