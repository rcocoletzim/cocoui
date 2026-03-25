/*
 * CocoUI Framework (https://github.com/rcocoletzim/cocoui)
 * Copyright (c) 2026 Rubén Cocoletzi Mata
 *
 * This software is provided 'as-is', under the terms of the zlib/libpng license.
 * See the LICENSE file in the project root for full license information.
 * SPDX-License-Identifier: Zlib
 */

#include <raylib.h>

#include <iostream>

#include "cocoui/framebuffer.hpp"
#include "cocoui/layout/column.hpp"
#include "cocoui/primitives.hpp"
#include "cocoui/widget.hpp"
#include "cocoui/widgets/solid_color.hpp"

constexpr int16_t kSimWidth = 320;
constexpr int16_t kSimHeight = 240;
constexpr float kPcScale = 2.0F;

constexpr auto color_to_raylib_hex(const cocoui::Color& color) -> uint32_t {
    return (static_cast<uint32_t>(color.a) << 24) | (static_cast<uint32_t>(color.b) << 16) |
           (static_cast<uint32_t>(color.g) << 8) | static_cast<uint32_t>(color.r);
}

auto main() -> int {
    std::cout << "[Simulator] Booting CocoUI Hardware Abstraction Layer...\n";

    InitWindow(static_cast<int>(kSimWidth * kPcScale), static_cast<int>(kSimHeight * kPcScale),
               "CocoUI - Hardware Simulator");
    SetTargetFPS(60);

    ::Image dummy_image = GenImageColor(kSimWidth, kSimHeight, ::BLANK);
    Texture2D display_texture = LoadTextureFromImage(dummy_image);
    UnloadImage(dummy_image);

    cocoui::Framebuffer<kSimWidth, kSimHeight, cocoui::PixelFormat::ARGB8888> framebuffer;

    // =========================================================================
    // 🎨 DECLARATIVE UI DEFINITION (Zero-Allocation Scene Graph)
    // =========================================================================
    // This entire UI tree is constructed at compile-time/stack. No heap used!
    auto app_ui = cocoui::make_column(cocoui::make_solid_color(cocoui::Colors::Red),
                                      cocoui::make_solid_color(cocoui::Colors::Green),
                                      cocoui::make_solid_color(cocoui::Colors::Blue));

    // Fluent API: Position the column with a 10px margin
    app_ui.bounds(cocoui::Rect(10, 10, kSimWidth - 20, kSimHeight - 20));

    // =========================================================================

    while (!WindowShouldClose()) {
        // --- RENDERING ---
        // 1. Clear background
        framebuffer.clear(color_to_raylib_hex(cocoui::Color(30, 30, 30)));

        // 2. Draw the entire Scene Graph recursively with a single call
        app_ui.draw(framebuffer, cocoui::Rect(0, 0, kSimWidth, kSimHeight));

        // --- DISPLAY UPDATE ---
        UpdateTexture(display_texture, framebuffer.data());

        BeginDrawing();
        ClearBackground(::BLACK);
        // Note: Designated initializers {.x=0, .y=0} are C++20, Raylib expects a struct in C++14
        Vector2 origin = {0.0F, 0.0F};
        DrawTextureEx(display_texture, origin, 0.0F, kPcScale, ::WHITE);
        EndDrawing();
    }

    UnloadTexture(display_texture);
    CloseWindow();
    return 0;
}
