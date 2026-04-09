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

namespace cocoui {

/**
 * @brief Base state for all UI elements.
 * Uses CRTP/Static Polymorphism instead of virtual methods for zero-overhead.
 */
 template <typename Derived>
class Widget {
   protected:
    Rect bounds_{0, 0, 0, 0};
    bool is_visible_{true};

   public:
    constexpr Widget() = default;
    constexpr explicit Widget(const Rect& bounds) : bounds_(bounds) {}

    constexpr Derived& at(Point origin) noexcept {
        bounds_.origin = origin;
        return static_cast<Derived&>(*this);
    }

    constexpr Derived& size(Size s) noexcept {
        bounds_.size = s;
        return static_cast<Derived&>(*this);
    }

    constexpr Derived& bounds(const Rect& r) noexcept {
        bounds_ = r;
        return static_cast<Derived&>(*this);
    }

    constexpr Derived& visible(bool v) noexcept {
        is_visible_ = v;
        return static_cast<Derived&>(*this);
    }

    [[nodiscard]] constexpr const Rect& get_bounds() const noexcept { return bounds_; }
    [[nodiscard]] constexpr bool is_visible() const noexcept { return is_visible_; }
};

}  // namespace cocoui
