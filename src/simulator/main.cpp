/*
 * CocoUI Framework
 * SPDX-License-Identifier: Zlib
 */

#include <raylib.h>
#include <iostream>

#include "cocoui/framebuffer.hpp"
#include "cocoui/layout/canvas.hpp"
#include "cocoui/primitives.hpp"
#include "cocoui/widget.hpp"
#include "cocoui/widgets/button.hpp"  
#include "cocoui/widgets/solid_color.hpp"
#include "cocoui/widgets/image.hpp" // <--- Include the new Image widget

constexpr int16_t kSimWidth = 320;
constexpr int16_t kSimHeight = 240;
constexpr float kPcScale = 2.0F;

// ============================================================================
// SIMULATED FLASH ASSETS (ROM)
// ============================================================================
constexpr uint32_t T = 0x00000000; // Transparent
constexpr uint32_t Y = 0xFFFFD700; // Yellow
constexpr uint32_t B = 0xFF000000; // Black

// An 8x8 pixel art smiley face
const uint32_t smiley_data[64] = {
    T, T, Y, Y, Y, Y, T, T,
    T, Y, Y, Y, Y, Y, Y, T,
    Y, Y, B, Y, Y, B, Y, Y,
    Y, Y, B, Y, Y, B, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y,
    Y, B, Y, Y, Y, Y, B, Y,
    T, Y, B, B, B, B, Y, T,
    T, T, Y, Y, Y, Y, T, T
};

const cocoui::Bitmap smiley_asset = { cocoui::Size(8, 8), smiley_data };
// ============================================================================

constexpr auto color_to_raylib_hex(const cocoui::Color& color) -> uint32_t {
    return (static_cast<uint32_t>(color.a) << 24) | (static_cast<uint32_t>(color.b) << 16) |
           (static_cast<uint32_t>(color.g) << 8) | static_cast<uint32_t>(color.r);
}

auto main() -> int {
    std::cout << "[Simulator] Booting CocoUI...\n";
    InitWindow(static_cast<int>(kSimWidth * kPcScale), static_cast<int>(kSimHeight * kPcScale),
               "CocoUI Simulator");
    SetTargetFPS(60);

    ::Image dummy_image = GenImageColor(kSimWidth, kSimHeight, ::BLANK);
    Texture2D display_texture = LoadTextureFromImage(dummy_image);
    UnloadImage(dummy_image);

    cocoui::Framebuffer<kSimWidth, kSimHeight, cocoui::PixelFormat::ARGB8888> framebuffer;
    cocoui::Color interactive_color = cocoui::Colors::Red;

    while (!WindowShouldClose()) {
        auto touch_x = static_cast<int16_t>(GetMouseX() / kPcScale);
        auto touch_y = static_cast<int16_t>(GetMouseY() / kPcScale);
        bool is_clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        // --- DECLARATIVE UI ---
        auto app_ui = cocoui::make_canvas(
            
            // Interactive Button
            cocoui::make_button(
                cocoui::make_solid_color(interactive_color), 
                [&interactive_color]() {
                    interactive_color = (interactive_color.r == 255) ? cocoui::Colors::Blue : cocoui::Colors::Red;
                }
            ).at({100, 100}).size({120, 40}), 

            // Green background box
            cocoui::make_solid_color(cocoui::Colors::Green)
                .at({10, 190}).size({40, 40}),

            // The new Image Widget displaying our Flash memory asset
            cocoui::make_image(smiley_asset).at({26, 206})
        );
        
        app_ui.bounds(cocoui::Rect(0, 0, kSimWidth, kSimHeight));

        if (is_clicked) {
            app_ui.handle_touch(cocoui::Point(touch_x, touch_y),
                                cocoui::Rect(0, 0, kSimWidth, kSimHeight));
        }

        framebuffer.clear(color_to_raylib_hex(cocoui::Color(30, 30, 30)));
        app_ui.draw(framebuffer, cocoui::Rect(0, 0, kSimWidth, kSimHeight));

        UpdateTexture(display_texture, framebuffer.data());
        BeginDrawing();
        ClearBackground(::BLACK);
        DrawTextureEx(display_texture, {0.0F, 0.0F}, 0.0F, kPcScale, ::WHITE);
        EndDrawing();
    }

    UnloadTexture(display_texture);
    CloseWindow();
    return 0;
}