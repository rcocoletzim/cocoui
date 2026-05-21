/*
 * CocoUI Framework
 * SPDX-License-Identifier: Zlib
 */

#pragma once

#include "cocoui/primitives.hpp"

namespace cocoui {
namespace hal {

/**
 * @brief Hardware Abstraction Layer for Displays.
 * The core UI engine does not care about the underlying hardware (SPI, I2C, Parallel, PC Simulator).
 * It simply hands over the calculated pixels to this interface.
 * * @tparam PixelType The memory representation of a single pixel (e.g., RGB565 or ARGB8888).
 */
template <typename PixelType>
class DisplayDriver {
   public:
    virtual ~DisplayDriver() = default;

    /**
     * @brief Transmits a rectangular block of pixels to the physical display hardware.
     * In an embedded system, this typically triggers a DMA transfer over SPI/Parallel bus.
     * * @param area The physical bounds on the screen where the pixels should be drawn.
     * @param color_data Pointer to the raw pixel array to be transmitted.
     */
    virtual void flush_area(const Rect& area, const PixelType* color_data) = 0;
};

}  // namespace hal
}  // namespace cocoui