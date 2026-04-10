/*
 * CocoUI Framework
 * SPDX-License-Identifier: Zlib
 */

#pragma once

#include <utility>

#include "cocoui/mixins/touchable.hpp"
#include "cocoui/primitives.hpp"
#include "cocoui/widget.hpp"

namespace cocoui {

// Button inherits both Layout capabilities (Widget) and Event capabilities (Touchable)
template <typename Visual, typename Callback>
class Button : public Widget<Button<Visual, Callback>>, 
               public Touchable<Callback> {
   private:
    Visual visual_;

   public:
    constexpr explicit Button(Visual visual, Callback on_click)
        : Widget<Button<Visual, Callback>>(),
          Touchable<Callback>(std::move(on_click)), 
          visual_(std::move(visual)) {}

    template <typename FB>
    void draw(FB& fb, const Rect& parent_bounds) const {
        if (!this->is_visible_) return;
        
        Rect absolute_bounds(
            parent_bounds.origin.x + this->bounds_.origin.x, parent_bounds.origin.y + this->bounds_.origin.y,
            this->bounds_.size.width > 0 ? this->bounds_.size.width : parent_bounds.size.width,
            this->bounds_.size.height > 0 ? this->bounds_.size.height : parent_bounds.size.height);
        
        visual_.draw(fb, absolute_bounds);
    }

    // Required by the SFINAE dispatcher. Delegates hit-testing to the Mixin.
    bool handle_touch(Point p, const Rect& parent_bounds) {
        if (!this->is_visible_) return false;

        Rect absolute_bounds(
            parent_bounds.origin.x + this->bounds_.origin.x, parent_bounds.origin.y + this->bounds_.origin.y,
            this->bounds_.size.width > 0 ? this->bounds_.size.width : parent_bounds.size.width,
            this->bounds_.size.height > 0 ? this->bounds_.size.height : parent_bounds.size.height);

        return this->process_touch(p, absolute_bounds);
    }
};

template <typename Visual, typename Callback>
constexpr auto make_button(Visual visual, Callback on_click) {
    return Button<Visual, Callback>(std::move(visual), std::move(on_click));
}

}  // namespace cocoui