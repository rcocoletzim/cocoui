/*
 * CocoUI Framework (https://github.com/rcocoletzim/cocoui)
 * Copyright (c) 2026 Rubén Cocoletzi Mata
 *
 * This software is provided 'as-is', under the terms of the zlib/libpng license.
 * See the LICENSE file in the project root for full license information.
 * SPDX-License-Identifier: Zlib
 */

#pragma once

#include <cstdint>

namespace cocoui {

// ============================================================================
// COLOR PRIMITIVE
// ============================================================================
struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    // Default constructor (Black, fully opaque)
    constexpr Color() : r(0), g(0), b(0), a(255) {}

    // RGBA Constructor
    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}

    // Hex Constructor (e.g., 0xFF0000FF for Red)
    // Format: 0xAARRGGBB
    constexpr explicit Color(uint32_t hex)
        : r((hex >> 16) & 0xFF), g((hex >> 8) & 0xFF), b(hex & 0xFF), a((hex >> 24) & 0xFF) {}

    // Helper to convert to a 32-bit integer (useful for Raylib and Framebuffers)
    [[nodiscard]] constexpr auto to_hex() const -> uint32_t {
        return (a << 24) | (r << 16) | (g << 8) | b;
    }
};

// Standard predefined colors computed at compile-time
namespace Colors {
constexpr Color Black(0, 0, 0);
constexpr Color White(255, 255, 255);
constexpr Color Red(255, 0, 0);
constexpr Color Green(0, 255, 0);
constexpr Color Blue(0, 0, 255);
constexpr Color Transparent(0, 0, 0, 0);
}  // namespace Colors

// ============================================================================
// POINT PRIMITIVE
// ============================================================================
struct Point {
    int16_t x;
    int16_t y;

    constexpr Point() : x(0), y(0) {}
    constexpr Point(int16_t x, int16_t y) : x(x), y(y) {}
};

// ============================================================================
// SIZE PRIMITIVE
// ============================================================================
struct Size {
    int16_t width;
    int16_t height;

    constexpr Size() : width(0), height(0) {}
    constexpr Size(int16_t w, int16_t h) : width(w), height(h) {}
};

// ============================================================================
// RECTANGLE PRIMITIVE
// ============================================================================
struct Rect {
    Point origin;
    Size size;

    constexpr Rect() = default;
    constexpr Rect(int16_t x, int16_t y, int16_t w, int16_t h) : origin(x, y), size(w, h) {}
    constexpr Rect(Point p, Size s) : origin(p), size(s) {}

    // Helper methods for easy coordinate calculations
    [[nodiscard]] constexpr auto left() const -> int16_t { return origin.x; }
    [[nodiscard]] constexpr auto right() const -> int16_t { return origin.x + size.width; }
    [[nodiscard]] constexpr auto top() const -> int16_t { return origin.y; }
    [[nodiscard]] constexpr auto bottom() const -> int16_t { return origin.y + size.height; }

    // Vital for Touch/Click detection in our IMGUI / Declarative approach
    [[nodiscard]] constexpr auto contains(const Point& p) const -> bool {
        return p.x >= left() && p.x < right() && p.y >= top() && p.y < bottom();
    }

    // Checks if two rectangles overlap (useful for Dirty Rectangles rendering)
    [[nodiscard]] constexpr auto intersects(const Rect& other) const -> bool {
        return other.left() < right() && other.right() > left() && other.top() < bottom() &&
               other.bottom() > top();
    }

    [[nodiscard]] constexpr auto intersection(const Rect& other) const -> Rect {
        // Use pure ternary operators to avoid std::max/std::min reference binding issues 
        // with temporary prvalues returned by left(), top(), etc. in a constexpr context.
        int16_t x1 = (left() > other.left()) ? left() : other.left();
        int16_t y1 = (top() > other.top()) ? top() : other.top();
        int16_t x2 = (right() < other.right()) ? right() : other.right();
        int16_t y2 = (bottom() < other.bottom()) ? bottom() : other.bottom();

        if (x1 < x2 && y1 < y2) {
            // Explicit static_cast neutralizes C++ Integral Promotion (int16_t -> int32_t)
            // ensuring the constructor receives strict 16-bit values.
            return {x1, y1, 
                        static_cast<int16_t>(x2 - x1), 
                        static_cast<int16_t>(y2 - y1)};
        }
        return {0, 0, 0, 0}; // No overlap
    }
};

// ============================================================================
// EVENT RESULT
// ============================================================================
// Replaces 'bool' to report exactly WHICH area of the screen got damaged
struct EventResult {
    bool consumed;
    Rect dirty_area;

    constexpr EventResult() : consumed(false), dirty_area(0, 0, 0, 0) {}
    constexpr EventResult(bool c, Rect r) : consumed(c), dirty_area(r) {}
};
}  // namespace cocoui
