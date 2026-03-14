#pragma once

#include "cocoui/primitives.hpp"

// Forward declaration of a generic drawing context (we'll implement this later)
namespace cocoui {
class Framebuffer;
}

namespace cocoui {

// A primitive widget that simply fills its assigned area with a color
class SolidColor {
   private:
    Color color_;

   public:
    // constexpr ensures this widget can be constructed at compile-time
    constexpr explicit SolidColor(Color c) : color_(c) {}

    // Every widget must have this signature.
    void draw(Framebuffer& fb, const Rect& bounds) const {
        // Here we will eventually call fb.fill_rect(bounds, color_);
        // For now, it's just the structural contract.
    }
};

// Factory helper for C++14
constexpr SolidColor make_solid_color(Color c) {
    return SolidColor(c);
}

}  // namespace cocoui