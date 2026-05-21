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
#include "cocoui/widgets/image.hpp"

// Include our new Hardware Abstraction Layer
#include "cocoui/hal/display_driver.hpp"

constexpr int16_t kSimWidth = 320;
constexpr int16_t kSimHeight = 240;
constexpr float kPcScale = 2.0F;

// ============================================================================
// RAYLIB FAKE HARDWARE DRIVER
// ============================================================================
// This class implements our HAL contract. The CocoUI engine will talk to this
// class without knowing it is actually interacting with a PC GPU via Raylib.
class RaylibDriver : public cocoui::hal::DisplayDriver<uint32_t> {
   private:
    Texture2D& texture_;

   public:
    explicit RaylibDriver(Texture2D& tex) : texture_(tex) {}

    // The engine calls this when it's time to send pixels to the screen
    void flush_area(const cocoui::Rect& /*area*/, const uint32_t* color_data) override {
        // In a real embedded system (SPI/DMA), you would use the 'area' parameter 
        // to only send a specific subset of pixels to the display controller.
        // For the PC simulator, we simply update the entire GPU texture.
        UpdateTexture(texture_, color_data);
    }
};
// ============================================================================

// Simulated Flash Asset
constexpr uint32_t T = 0x00000000;
constexpr uint32_t Y = 0xFFFFD700;
constexpr uint32_t B = 0xFF000000;
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

    // 1. Initialize Framebuffer (Software Memory)
    cocoui::Framebuffer<kSimWidth, kSimHeight, cocoui::PixelFormat::ARGB8888> framebuffer;
    
    // ... [setup raylib and framebuffer] ...
    RaylibDriver driver(display_texture);
    cocoui::Color interactive_color = cocoui::Colors::Red;

    // --- INITIAL FULL DRAW (Done once) ---
    auto app_ui = cocoui::make_canvas(
        cocoui::make_button(
            cocoui::make_solid_color(interactive_color), 
            [&interactive_color]() {
                interactive_color = (interactive_color.r == 255) ? cocoui::Colors::Blue : cocoui::Colors::Red;
            }
        ).at({100, 100}).size({120, 40}), 
        cocoui::make_solid_color(cocoui::Colors::Green).at({10, 190}).size({40, 40})
    );
    app_ui.bounds(cocoui::Rect(0, 0, kSimWidth, kSimHeight));

    framebuffer.reset_clip(); // Draw everywhere
    framebuffer.clear(color_to_raylib_hex(cocoui::Color(30, 30, 30)));
    app_ui.draw(framebuffer, cocoui::Rect(0, 0, kSimWidth, kSimHeight));
    framebuffer.flush(driver);
    UpdateTexture(display_texture, framebuffer.data());

    // --- MAIN IDLE LOOP ---
    bool needs_redraw = false;

    while (!WindowShouldClose()) {
        auto touch_x = static_cast<int16_t>(GetMouseX() / kPcScale);
        auto touch_y = static_cast<int16_t>(GetMouseY() / kPcScale);

        // Process Input
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            auto result = app_ui.handle_touch(cocoui::Point(touch_x, touch_y),
                                              cocoui::Rect(0, 0, kSimWidth, kSimHeight));
            // DID SOMETHING GET DIRTY?
            if (result.consumed) {
                framebuffer.set_clip(result.dirty_area); // Mask the screen
                needs_redraw = true;
            }
        }

        // Draw ONLY if dirty
        if (needs_redraw) {
            // Rebuild UI with new color state
            auto dynamic_ui = cocoui::make_canvas(
                cocoui::make_button(
                    cocoui::make_solid_color(interactive_color), 
                    [&interactive_color]() {
                        interactive_color = (interactive_color.r == 255) ? cocoui::Colors::Blue : cocoui::Colors::Red;
                    }
                ).at({100, 100}).size({120, 40}), 
                cocoui::make_solid_color(cocoui::Colors::Green).at({10, 190}).size({40, 40})
            );
            dynamic_ui.bounds(cocoui::Rect(0, 0, kSimWidth, kSimHeight));

            // Clear and draw ONLY inside the clip_rect_
            framebuffer.clear(color_to_raylib_hex(cocoui::Color(30, 30, 30)));
            dynamic_ui.draw(framebuffer, cocoui::Rect(0, 0, kSimWidth, kSimHeight));
            
            // Send the tiny patch to hardware
            framebuffer.flush(driver); 
            UpdateTexture(display_texture, framebuffer.data());
            
            needs_redraw = false;
        }

        // Display the GPU texture to the PC window
        BeginDrawing();
        ClearBackground(::BLACK);
        DrawTextureEx(display_texture, {0.0F, 0.0F}, 0.0F, kPcScale, ::WHITE);
        EndDrawing();
    }

    UnloadTexture(display_texture);
    CloseWindow();
    return 0;
}