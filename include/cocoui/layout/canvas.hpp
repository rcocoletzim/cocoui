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
class Canvas : public Widget<Canvas<Children...>> {
   private:
    // Tuple saves all in stack/flash
    std::tuple<Children...> children_;

    template <typename FB, std::size_t... Indices>
    void draw_impl(FB& fb, const Rect& parent_bounds, std::index_sequence<Indices...>) const {
        using expander = int[];

        // absolute position of canves in screen
        int16_t canvas_abs_x = parent_bounds.origin.x + bounds_.origin.x;
        int16_t canvas_abs_y = parent_bounds.origin.y + bounds_.origin.y;
        
        int16_t canvas_w = bounds_.size.width > 0 ? bounds_.size.width : parent_bounds.size.width;
        int16_t canvas_h = bounds_.size.height > 0 ? bounds_.size.height : parent_bounds.size.height;


        (void)expander{0,
                       (std::get<Indices>(children_).draw(
                            fb, Rect(canvas_abs_x, canvas_abs_y, canvas_w, canvas_h)),
                        0)...};
    }

    template <std::size_t... Indices>
    bool handle_touch_impl(Point p, const Rect& parent_bounds, std::index_sequence<Indices...>) {
        int16_t canvas_abs_x = parent_bounds.origin.x + bounds_.origin.x;
        int16_t canvas_abs_y = parent_bounds.origin.y + bounds_.origin.y;
        
        int16_t canvas_w = bounds_.size.width > 0 ? bounds_.size.width : parent_bounds.size.width;
        int16_t canvas_h = bounds_.size.height > 0 ? bounds_.size.height : parent_bounds.size.height;

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
    constexpr explicit Canvas(Children... children) : children_(std::move(children)...) {}

    template <typename FB>
    void draw(FB& fb, const Rect& parent_bounds) const {
        if (!is_visible_) return;
        draw_impl(fb, parent_bounds, std::index_sequence_for<Children...>{});
    }

    bool handle_touch(Point p, const Rect& parent_bounds) {
        if (!is_visible_) return false;
        return handle_touch_impl(p, parent_bounds, std::index_sequence_for<Children...>{});
    }
};

template <typename... Children>
constexpr Canvas<Children...> make_canvas(Children... children) {
    return Canvas<Children...>(std::move(children)...);
}

}  // namespace cocoui