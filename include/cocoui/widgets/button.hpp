/*
 * CocoUI Framework
 * SPDX-License-Identifier: Zlib
 */

#pragma once

#include <utility>

#include "cocoui/primitives.hpp"
#include "cocoui/widget.hpp"

namespace cocoui {

template <typename Visual, typename Callback>
class Button : public Widget<Button<Visual, Callback>> {
   private:
    Visual visual_;
    Callback on_click_;

   public:
    constexpr explicit Button(Visual visual, Callback on_click)
        : visual_(std::move(visual)), on_click_(std::move(on_click)) {}

    template <typename FB>
    void draw(FB& fb, const Rect& parent_bounds) const {
        if (!is_visible_) return;
        
        Rect absolute_bounds(
            parent_bounds.origin.x + bounds_.origin.x, parent_bounds.origin.y + bounds_.origin.y,
            bounds_.size.width > 0 ? bounds_.size.width : parent_bounds.size.width,
            bounds_.size.height > 0 ? bounds_.size.height : parent_bounds.size.height);
        visual_.draw(fb, absolute_bounds);
    }

    bool handle_touch(Point p, const Rect& parent_bounds) {
        if (!is_visible_) return false;

        Rect absolute_bounds(
            parent_bounds.origin.x + bounds_.origin.x, parent_bounds.origin.y + bounds_.origin.y,
            bounds_.size.width > 0 ? bounds_.size.width : parent_bounds.size.width,
            bounds_.size.height > 0 ? bounds_.size.height : parent_bounds.size.height);

        if (absolute_bounds.contains(p)) {
            on_click_();  
            return true;  
        }
        return false;
    }
};

template <typename Visual, typename Callback>
constexpr auto make_button(Visual visual, Callback on_click) {
    return Button<Visual, Callback>(std::move(visual), std::move(on_click));
}

}  // namespace cocoui