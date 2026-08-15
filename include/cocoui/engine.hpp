/*
 * CocoUI Framework - Core Engine
 * SPDX-License-Identifier: Zlib
 */

#pragma once

#include "cocoui/framebuffer.hpp"
#include "cocoui/hal/display_driver.hpp"

namespace cocoui {

/**
 * @brief The core engine that manages the UI lifecycle.
 * Encapsulates the Framebuffer, Hardware Driver, and Application State.
 */
template <typename State, typename ViewBuilder, int16_t W, int16_t H, typename P>
class Engine {
   private:
    State state_;
    ViewBuilder builder_;
    Framebuffer<W, H, P>& fb_;
    hal::DisplayDriver<P>& driver_;
    Color bg_color_;
    bool needs_redraw_{true};

   public:
    constexpr Engine(State state, ViewBuilder builder, Framebuffer<W, H, P>& fb, 
                     hal::DisplayDriver<P>& driver, Color bg_color = Colors::Black)
        : state_(state), builder_(builder), fb_(fb), driver_(driver), bg_color_(bg_color) {}

    /**
     * @brief Performs the initial full-screen render loop.
     */
    void start() {
        fb_.reset_clip();
        fb_.clear(ColorConverter<P>::convert(bg_color_));
        
        // Build, draw, and discard the UI tree immediately in the Stack
        auto initial_ui = builder_(state_);
        initial_ui.draw(fb_, Rect(0, 0, W, H));
        
        fb_.flush(driver_);
        needs_redraw_ = false;
    }

    /**
     * @brief Injects hardware touch events into the engine.
     */
    void inject_touch(bool is_touched, int16_t x, int16_t y) {
        if (is_touched) {
            // Build a temporary tree just to evaluate coordinates
            auto temporary_ui = builder_(state_);
            auto result = temporary_ui.handle_touch(Point(x, y), Rect(0, 0, W, H));
            
            // If any widget consumed the touch event, mark its area as dirty.
            if (result.consumed) {
                fb_.set_clip(result.dirty_area);
                needs_redraw_ = true;
            }
        }
    }

    /**
     * @brief Processes the current state. If a redraw is needed, updates only the dirty area.
     */
    void tick() {
        if (needs_redraw_) {
            fb_.clear(ColorConverter<P>::convert(bg_color_));
            
            auto active_ui = builder_(state_); 
            active_ui.draw(fb_, Rect(0, 0, W, H));
            
            fb_.flush(driver_);
            needs_redraw_ = false;
        }
    }
};

// Factory helper to avoid heavy template syntax during instantiation.
template <int16_t W, int16_t H, typename P, typename State, typename ViewBuilder>
constexpr auto make_engine(State init_state, ViewBuilder builder, 
                           Framebuffer<W, H, P>& fb, hal::DisplayDriver<P>& driver, 
                           Color bg = Colors::Black) {
    return Engine<State, ViewBuilder, W, H, P>(init_state, builder, fb, driver, bg);
}

}  // namespace cocoui