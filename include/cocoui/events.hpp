/*
 * CocoUI Framework
 * SPDX-License-Identifier: Zlib
 */

#pragma once

#include <type_traits>
#include "cocoui/primitives.hpp"

namespace cocoui {

namespace detail {
    // C++14 compliant implementation of void_t used for SFINAE detection
    template <typename... Ts> struct make_void { typedef void type; };
    template <typename... Ts> using void_t = typename make_void<Ts...>::type;

    // Primary template: assumes type T does NOT have a handle_touch method
    template <typename T, typename = void>
    struct is_touchable : std::false_type {};

    // Specialization: if calling T::handle_touch is valid, this becomes true_type
    template <typename T>
    struct is_touchable<T, void_t<decltype(std::declval<T>().handle_touch(Point{}, Rect{}))>> : std::true_type {};
} // namespace detail

// ============================================================================
// STATIC DISPATCHERS
// ============================================================================

// Overload 1: Widget HAS handle_touch (e.g., Button)
template <typename T>
constexpr auto dispatch_touch(T& widget, Point p, const Rect& bounds) 
    -> typename std::enable_if<detail::is_touchable<T>::value, bool>::type {
    return widget.handle_touch(p, bounds);
}

// Overload 2: Widget DOES NOT have handle_touch (e.g., SolidColor, Image)
template <typename T>
constexpr auto dispatch_touch(T& /*widget*/, Point /*p*/, const Rect& /*bounds*/) 
    -> typename std::enable_if<!detail::is_touchable<T>::value, bool>::type {
    return false; // Safely ignore at compile-time
}

} // namespace cocoui