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
#include "cocoui/primitives.hpp"
#include "cocoui/framebuffer.hpp"

// We avoid 'using namespace cocoui' to prevent name collisions with Raylib
// Instead, we use specific aliases or prefix with 'cocoui::'

// Simulator Constants with descriptive names to avoid "magic numbers" warnings
constexpr int16_t kSimWidth = 320;
constexpr int16_t kSimHeight = 240;
constexpr float kPcScale = 2.0F; 

/**
 * @brief Maps CocoUI Color to Raylib Hardware Hex (RGBA)
 */
constexpr auto color_to_raylib_hex(const cocoui::Color& color) -> uint32_t {
    return (static_cast<uint32_t>(color.a) << 24) | 
           (static_cast<uint32_t>(color.b) << 16) | 
           (static_cast<uint32_t>(color.g) << 8)  | 
            static_cast<uint32_t>(color.r);
}

auto main() -> int {
    std::cout << "[Simulator] Booting CocoUI Hardware Abstraction Layer...\n";

    InitWindow(static_cast<int>(kSimWidth * kPcScale), 
               static_cast<int>(kSimHeight * kPcScale), 
               "CocoUI - Hardware Simulator");
    SetTargetFPS(60);

    // 1. GPU Texture setup
    // Explicitly use ::Color to tell Clang we want Raylib's Color struct here
    ::Image dummy_image = GenImageColor(kSimWidth, kSimHeight, ::BLANK);
    Texture2D display_texture = LoadTextureFromImage(dummy_image);
    UnloadImage(dummy_image); 

    // 2. CocoUI Framebuffer 
    // Explicitly using cocoui::PixelFormat to avoid collision with Raylib's
    cocoui::Framebuffer<kSimWidth, kSimHeight, cocoui::PixelFormat::ARGB8888> framebuffer;

    while (!WindowShouldClose()) {
        // --- A. INPUT ---
        auto touch_x = static_cast<int16_t>(GetMouseX() / kPcScale);
        auto touch_y = static_cast<int16_t>(GetMouseY() / kPcScale);
        bool is_touching = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

        // --- B. RENDERING ---
        // Clear background (Dark Gray)
        framebuffer.clear(color_to_raylib_hex(cocoui::Color(30, 30, 30)));

        // Top Bar
        framebuffer.fill_rect(cocoui::Rect(0, 0, kSimWidth, 30), 
                              color_to_raylib_hex(cocoui::Colors::Blue));

        // Interactive Red Square
        if (is_touching) {
            framebuffer.fill_rect(cocoui::Rect(touch_x - 20, touch_y - 20, 40, 40), 
                                  color_to_raylib_hex(cocoui::Colors::Red));
        }

        // --- C. DISPLAY UPDATE ---
        UpdateTexture(display_texture, framebuffer.data());

        BeginDrawing();
            ClearBackground(::BLACK); // Use :: to refer to Raylib's global macro
            DrawTextureEx(display_texture, {.x=0.0F, .y=0.0F}, 0.0F, kPcScale, ::WHITE);
        EndDrawing();
    }

    UnloadTexture(display_texture);
    CloseWindow();
    return 0;
}