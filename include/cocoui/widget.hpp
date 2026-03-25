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
class Widget {
   protected:
    Rect bounds_{0, 0, 0, 0};
    bool is_visible_{true};

   public:
    constexpr Widget() = default;
    constexpr explicit Widget(const Rect& bounds) : bounds_(bounds) {}

    constexpr Widget& at(Point origin) noexcept {
        bounds_.origin = origin;
        return *this;
    }

    constexpr Widget& size(Size s) noexcept {
        bounds_.size = s;
        return *this;
    }

    constexpr Widget& bounds(const Rect& r) noexcept {
        bounds_ = r;
        return *this;
    }

    constexpr Widget& visible(bool v) noexcept {
        is_visible_ = v;
        return *this;
    }

    [[nodiscard]] constexpr const Rect& get_bounds() const noexcept { return bounds_; }
    [[nodiscard]] constexpr bool is_visible() const noexcept { return is_visible_; }
};

}  // namespace cocoui
