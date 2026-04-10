/*
 * CocoUI Framework
 * SPDX-License-Identifier: Zlib
 */

#pragma once

#include <utility>
#include "cocoui/primitives.hpp"

namespace cocoui {

/**
 * @brief Mixin policy that encapsulates callback execution and hit-testing logic.
 * Any widget inheriting this gains the ability to process touch bounds safely.
 */
template <typename Callback>
class Touchable {
   protected:
    Callback on_click_;

   public:
    constexpr explicit Touchable(Callback cb) : on_click_(std::move(cb)) {}

    // Evaluates if the point is inside the absolute bounds and triggers the callback
    bool process_touch(Point p, const Rect& absolute_bounds) {
        if (absolute_bounds.contains(p)) {
            on_click_();
            return true; // Event consumed
        }
        return false; // Event ignored
    }
};

} // namespace cocoui