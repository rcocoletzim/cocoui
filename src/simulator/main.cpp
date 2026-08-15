/*
 * CocoUI Framework - PC Simulator
 * SPDX-License-Identifier: Zlib
 */

#include <raylib.h>
#include <iostream>

#include "cocoui/engine.hpp"
#include "cocoui/dsl.hpp"
#include "cocoui/font.hpp"

constexpr int16_t kSimWidth = 320;
constexpr int16_t kSimHeight = 240;
constexpr float kPcScale = 2.0F;

// ============================================================================
// RAYLIB HARDWARE DRIVER (PC 32-bit ARGB)
// ============================================================================
class RaylibDriver : public cocoui::hal::DisplayDriver<uint32_t> {
   private:
    Texture2D& texture_;

   public:
    explicit RaylibDriver(Texture2D& tex) : texture_(tex) {}

    void flush_area(const cocoui::Rect& /*area*/, const uint32_t* color_data) override {
        // Send pixel array to the PC GPU
        UpdateTexture(texture_, color_data);
    }
};

// ============================================================================
// SIMULATED ASSETS (ROM)
// ============================================================================
constexpr uint32_t T = 0x00000000;
constexpr uint32_t Y = 0xFFFFD700;
constexpr uint32_t B = 0xFF000000;
const uint32_t smiley_data[64] = {
    T, T, Y, Y, Y, Y, T, T,  T, Y, Y, Y, Y, Y, Y, T,
    Y, Y, B, Y, Y, B, Y, Y,  Y, Y, B, Y, Y, B, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y,  Y, B, Y, Y, Y, Y, B, Y,
    T, Y, B, B, B, B, Y, T,  T, T, Y, Y, Y, Y, T, T
};
const cocoui::Bitmap smiley_asset = { cocoui::Size(8, 8), smiley_data };

const uint8_t abc_font_bitmap[] = {
    0x38, 0x6C, 0xC6, 0xFE, 0xC6, 0xC6, 0xC6, 0x00, // A
    0xFC, 0x66, 0x66, 0x7C, 0x66, 0x66, 0xFC, 0x00, // B
    0x3C, 0x66, 0xC0, 0xC0, 0xC0, 0x66, 0x3C, 0x00, // C
};
const cocoui::Font font_8x8 = { abc_font_bitmap, 8, 8, 'A', 'C' };

// ============================================================================
// APPLICATION DOMAIN (Declarative UI)
// ============================================================================
using namespace cocoui::dsl;
using namespace cocoui::Colors;

// 1. Application State
struct AppState {
    cocoui::Color current_theme = Red;
    int click_count = 0;
};

// 2. Purely Declarative View Builder
constexpr auto build_ui(AppState& state) {
    return Canvas(
        
        // Prefabricated Button with modular text offset position
        TextButton("C A B", font_8x8, state.current_theme, White, {30, 16}, [&state]() {
            state.click_count++;
            state.current_theme = (state.current_theme.r == 255) ? Blue : Red;
        }).at({100, 100}).size({120, 40}),

        TextButton("C B", font_8x8, state.current_theme, White, {30, 16}, [&state]() {
            state.click_count++;
            state.current_theme = (state.current_theme.r == 255) ? Blue : Red;
        }).at({100, 170}).size({120, 40}),

        // Static Decorations
        Box(Green).at({10, 190}).size({40, 40}),
        Picture(smiley_asset).at({26, 206}) // Updated to clean Name Collision

    ).bounds({0, 0, kSimWidth, kSimHeight});
}

// ============================================================================
// SIMULATOR ENTRY POINT
// ============================================================================
auto main() -> int {
    std::cout << "[Simulator] Booting CocoUI...\n";
    InitWindow(static_cast<int>(kSimWidth * kPcScale), static_cast<int>(kSimHeight * kPcScale), "CocoUI Simulator");
    SetTargetFPS(60);

    ::Image dummy_image = GenImageColor(kSimWidth, kSimHeight, ::BLANK);
    Texture2D display_texture = LoadTextureFromImage(dummy_image);
    UnloadImage(dummy_image);

    // 1. System Instantiation
    cocoui::Framebuffer<kSimWidth, kSimHeight, uint32_t> framebuffer;
    RaylibDriver driver(display_texture);
    AppState app_state;

    // 2. Engine Instantiation (Binds State, View, and Hardware)
    auto ui_engine = cocoui::make_engine(app_state, build_ui, framebuffer, driver, cocoui::Color(30, 30, 30));
    ui_engine.start();

    // 3. Main Event Loop
    while (!WindowShouldClose()) {
        // Poll Simulator Hardware Input
        bool is_touched = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        auto touch_x = static_cast<int16_t>(GetMouseX() / kPcScale);
        auto touch_y = static_cast<int16_t>(GetMouseY() / kPcScale);

        // Inject Events into the UI Engine
        ui_engine.inject_touch(is_touched, touch_x, touch_y);

        // Tick Engine (Recalculates and redraws ONLY if state changed)
        ui_engine.tick();

        // Render to PC Screen
        BeginDrawing();
        ClearBackground(::BLACK);
        DrawTextureEx(display_texture, {0.0F, 0.0F}, 0.0F, kPcScale, ::WHITE);
        EndDrawing();
    }

    UnloadTexture(display_texture);
    CloseWindow();
    return 0;
}