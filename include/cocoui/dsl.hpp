/*
 * CocoUI Framework - Domain Specific Language (DSL)
 * SPDX-License-Identifier: Zlib
 */

#pragma once

#include <utility>

#include "cocoui/layout/canvas.hpp"
#include "cocoui/layout/column.hpp"
#include "cocoui/layout/row.hpp"
#include "cocoui/widgets/button.hpp"
#include "cocoui/widgets/solid_color.hpp"
#include "cocoui/widgets/label.hpp"
#include "cocoui/widgets/image.hpp"

namespace cocoui {
namespace dsl {

    // ============================================================================
    // LAYOUTS
    // ============================================================================
    template <typename... Children>
    constexpr auto Canvas(Children... children) { 
        return make_canvas(std::move(children)...); 
    }
    
    template <typename... Children>
    constexpr auto VStack(Children... children) { 
        return make_column(std::move(children)...); 
    }
    
    template <typename... Children>
    constexpr auto HStack(Children... children) { 
        return make_row(std::move(children)...); 
    }

    // ============================================================================
    // VISUAL WIDGETS
    // ============================================================================
    constexpr auto Box(Color c) { 
        return make_solid_color(c); 
    }
    
    constexpr auto Text(const char* t, const Font& f, Color c) { 
        return make_label(t, f, c); 
    }
    
    constexpr auto Picture(Bitmap b) { 
        return make_image(b); 
    }

    // ============================================================================
    // PREFABRICATED COMPONENTS (Composed Widgets)
    // ============================================================================
    /**
     * @brief A ready-to-use button containing a background color and text at a specific position.
     */
    template <typename Callback>
    constexpr auto TextButton(const char* txt, const Font& font, Color bg_color, 
                              Color text_color, Point text_pos, Callback on_click) {
        return make_button(
            Canvas(
                Box(bg_color),
                Text(txt, font, text_color).at(text_pos) 
            ), 
            std::move(on_click)
        );
    }

} // namespace dsl
} // namespace cocoui