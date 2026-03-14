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

namespace cocoui {

template <typename... Children>
class Column {
   private:
    // The entire UI subtree is stored completely flat in memory
    std::tuple<Children...> children_;

    // Compile-time recursion helper for C++14 using the "Dummy Array Trick"
    template <std::size_t... Indices>
    void draw_impl(Framebuffer& fb, const Rect& bounds, std::index_sequence<Indices...>) const {
        // This array only exists to force the compiler to expand the parameter pack
        using expander = int[];

        // We unpack the tuple. For now, we just stack them vertically with a fixed height.
        // In a complete layout engine, we would compute sizes dynamically.
        (void)expander{0, (std::get<Indices>(children_).draw(
                               fb, Rect(bounds.origin.x, bounds.origin.y + (Indices * 40),
                                        bounds.size.width, 40)),
                           0)...};
    }

   public:
    // Constructor moves the children into the tuple
    constexpr explicit Column(Children... children) : children_(std::move(children)...) {}

    // Public draw interface
    void draw(Framebuffer& fb, const Rect& bounds) const {
        // Generate a compile-time sequence (0, 1, 2, ..., N-1) to iterate the tuple
        draw_impl(fb, bounds, std::index_sequence_for<Children...>{});
    }
};

// Factory function to deduce types automatically in C++14
template <typename... Children>
constexpr Column<Children...> make_column(Children... children) {
    return Column<Children...>(std::move(children)...);
}

}  // namespace cocoui