#include <raylib.h>

#include <iostream>

#include "cocoui/framebuffer.hpp"
#include "cocoui/layout/column.hpp"
#include "cocoui/primitives.hpp"
#include "cocoui/widget.hpp"
#include "cocoui/widgets/button.hpp"  
#include "cocoui/widgets/solid_color.hpp"

constexpr int16_t kSimWidth = 320;
constexpr int16_t kSimHeight = 240;
constexpr float kPcScale = 2.0F;

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

        auto app_ui = cocoui::make_column(
            cocoui::make_button(cocoui::make_solid_color(interactive_color),  // Apariencia visual
                                [&interactive_color]() {  // Acción: Función Lambda
                                    if (interactive_color.r == 255) {
                                        interactive_color = cocoui::Colors::Blue;
                                    } else {
                                        interactive_color = cocoui::Colors::Red;
                                    }
                                }),
            cocoui::make_solid_color(cocoui::Colors::Green));
        app_ui.bounds(cocoui::Rect(10, 10, kSimWidth - 20, kSimHeight - 20));

        if (is_clicked) {
            // El motor busca qué fue tocado y ejecuta la Lambda correcta
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