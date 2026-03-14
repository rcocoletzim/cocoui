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

   public:
    // Ensure the buffer is zeroed out at startup without runtime cost
    constexpr Framebuffer()  = default;

    // ========================================================================
    // HARDWARE / DRIVER INTERFACE
    // ========================================================================

    [[nodiscard]] constexpr auto data() const noexcept -> const PixelType* { return data_; }

    [[nodiscard]] constexpr auto size_bytes() const noexcept -> std::size_t { return buffer_size_bytes; }

    // ========================================================================
    // DRAWING OPERATIONS
    // ========================================================================

    /**
     * @brief Fills the entire screen with a specific pixel value.
     * Uses std::fill which the compiler lowers to highly optimized assembly (memset).
     */
    void clear(PixelType color_value) noexcept {
        std::fill(data_, data_ + pixel_count, color_value);
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
        // 1. Clipping: Calculate the visible intersection with the screen
        const int16_t x_start = std::max(static_cast<int16_t>(0), rect.left());
        const int16_t y_start = std::max(static_cast<int16_t>(0), rect.top());
        const int16_t x_end = std::min(width, rect.right());
        const int16_t y_end = std::min(height, rect.bottom());

        // 2. Early exit if the rectangle is completely off-screen
        if (x_start >= x_end || y_start >= y_end) {
            return;
        }

        // 3. Row-by-row rendering (Cache-friendly)
        const int16_t row_width = x_end - x_start;
        for (int16_t y = y_start; y < y_end; ++y) {
            PixelType* row_ptr = &data_[(y * width) + x_start];
            std::fill(row_ptr, row_ptr + row_width, color_value);
        }
    }
};

}  // namespace cocoui