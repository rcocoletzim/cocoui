/*
 * CocoUI Framework (https://github.com/rcocoletzim/cocoui)
 * Copyright (c) 2026 Rubén Cocoletzi Mata
 *
 * This software is provided 'as-is', under the terms of the zlib/libpng license.
 * See the LICENSE file in the project root for full license information.
 * SPDX-License-Identifier: Zlib
 */

#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>

#include "cocoui/primitives.hpp"
#include "cocoui/hal/display_driver.hpp"
namespace cocoui {

// ============================================================================
// PIXEL FORMAT DEFINITIONS
// ============================================================================
// Strongly typed pixel formats to prevent accidental color mixing
namespace PixelFormat {
using RGB565 = uint16_t;    // Standard for embedded LCDs (16-bit)
using ARGB8888 = uint32_t;  // Standard for PC Simulators / High-end GPUs (32-bit)
}  // namespace PixelFormat

// ============================================================================
// FRAMEBUFFER CORE
// ============================================================================
/**
 * @brief Professional Zero-Allocation Framebuffer.
 * * @tparam Width Screen width in pixels.
 * @tparam Height Screen height in pixels.
 * @tparam PixelType The memory representation of a single pixel (Default: ARGB8888).
 */
template <int16_t Width, int16_t Height, typename PixelType = PixelFormat::ARGB8888>
class Framebuffer {
   public:
    // Compile-time constants for layout calculations
    static constexpr int16_t width = Width;
    static constexpr int16_t height = Height;
    static constexpr std::size_t pixel_count = static_cast<std::size_t>(Width) * Height;
    static constexpr std::size_t buffer_size_bytes = pixel_count * sizeof(PixelType);

   private:
    // ALIGNMENT MAGIC:
    // ARM Cortex-M7/M33 architectures require buffers to be aligned to cache-line
    // boundaries (typically 32 bytes) for safe and fast DMA (Direct Memory Access) transfers.
    alignas(32) PixelType data_[pixel_count]{0};

    Rect clip_rect_{0, 0, Width, Height};

   public:
    // Ensure the buffer is zeroed out at startup without runtime cost
    constexpr Framebuffer() = default;

    // Clipping controls
    constexpr void set_clip(const Rect& r) noexcept { clip_rect_ = r; }
    constexpr void reset_clip() noexcept { clip_rect_ = Rect(0, 0, width, height); }

    // ========================================================================
    // HARDWARE / DRIVER INTERFACE
    // ========================================================================

    [[nodiscard]] constexpr auto data() const noexcept -> const PixelType* { return data_; }

    [[nodiscard]] constexpr auto size_bytes() const noexcept -> std::size_t {
        return buffer_size_bytes;
    }

    /**
     * @brief Sends the entire Framebuffer memory to the active display driver.
     * * @param driver A reference to the hardware-specific driver implementation.
     */
    void flush(hal::DisplayDriver<PixelType>& driver) const noexcept {
        // Send the full screen dimensions and the pointer to our aligned memory block
        driver.flush_area(clip_rect_, data_);
    }

    // ========================================================================
    // DRAWING OPERATIONS
    // ========================================================================

    /**
     * @brief Fills the entire screen with a specific pixel value.
     * Uses std::fill which the compiler lowers to highly optimized assembly (memset).
     */
    void clear(PixelType color_value) noexcept {
        fill_rect(clip_rect_, color_value);
    }

    /**
     * @brief Writes a single pixel with strict bounds checking.
     */
    void draw_pixel(int16_t x, int16_t y, PixelType color_value) noexcept {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            data_[(y * width) + x] = color_value;
        }
    }

    /**
     * @brief Renders a filled rectangle using row-major memory access.
     * Includes mathematical clipping to ensure no out-of-bounds memory writes occur.
     */
    void fill_rect(const Rect& rect, PixelType color_value) noexcept {
        // 1. Intersect requested rect with the allowed Clip Rect
        Rect target = rect.intersection(clip_rect_);
        
        // 2. Early exit if nothing to draw
        if (target.size.width <= 0 || target.size.height <= 0) return;

        // 3. Row-by-row rendering
        const int16_t row_width = target.size.width;
        for (int16_t y = target.top(); y < target.bottom(); ++y) {
            PixelType* row_ptr = &data_[(y * width) + target.left()];
            std::fill(row_ptr, row_ptr + row_width, color_value);
        }
    }

    /**
     * @brief Draws a raw bitmap directly from memory (Flash/ROM) with clipping support.
     */
    void draw_image(Point dest, const PixelType* src_data, Size src_size) noexcept {
        // 1. Clipping: Calculate the visible intersection with the screen boundaries
        int16_t x_start = std::max(static_cast<int16_t>(0), dest.x);
        int16_t y_start = std::max(static_cast<int16_t>(0), dest.y);
        int16_t x_end = std::min(width, static_cast<int16_t>(dest.x + src_size.width));
        int16_t y_end = std::min(height, static_cast<int16_t>(dest.y + src_size.height));

        // 2. Early exit if the image is completely off-screen
        if (x_start >= x_end || y_start >= y_end) {
            return;
        }

        // 3. Pixel-by-pixel rendering
        for (int16_t y = y_start; y < y_end; ++y) {
            int16_t src_y = y - dest.y;
            for (int16_t x = x_start; x < x_end; ++x) {
                int16_t src_x = x - dest.x;

                PixelType pixel = src_data[(src_y * src_size.width) + src_x];

                // Alpha Test: If the pixel is not completely transparent (Alpha > 0), draw it.
                // Note: >> 24 assumes ARGB8888 format for the PC simulator. 
                // In hardware, this will be abstracted to handle RGB565 color keying.
                if ((pixel >> 24) & 0xFF) {
                    data_[(y * width) + x] = pixel;
                }
            }
        }
    }

    /**
     * @brief Draws a monochrome (1-bit per pixel) glyph. 
     * Assumes widths up to 8 pixels (1 byte per row) for simplicity in this version.
     */
    void draw_glyph(Point dest, const uint8_t* glyph_data, uint8_t glyph_width, uint8_t glyph_height, PixelType color_value) noexcept {
        // 1. Clipping boundaries
        int16_t x_start = std::max(static_cast<int16_t>(0), dest.x);
        int16_t y_start = std::max(static_cast<int16_t>(0), dest.y);
        int16_t x_end = std::min(width, static_cast<int16_t>(dest.x + glyph_width));
        int16_t y_end = std::min(height, static_cast<int16_t>(dest.y + glyph_height));

        if (x_start >= x_end || y_start >= y_end) return;

        // 2. Decode the 1-bit mask and draw
        for (int16_t y = y_start; y < y_end; ++y) {
            int16_t src_y = y - dest.y;
            // Grab the row mask (1 byte = 8 pixels)
            uint8_t row_mask = glyph_data[src_y];
            
            for (int16_t x = x_start; x < x_end; ++x) {
                int16_t src_x = x - dest.x;
                
                // Read bits from MSB (Most Significant Bit) to LSB (Left to Right)
                if (row_mask & (1 << (7 - src_x))) {
                    data_[(y * width) + x] = color_value;
                }
            }
        }
    }
};

}  // namespace cocoui