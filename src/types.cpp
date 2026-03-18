#include "include/std/types.h"
#include <algorithm>

namespace VEOEngine {

Color Color::fromHex(uint32_t hex) {
    float r = ((hex >> 16) & 0xFF) / 255.0f;
    float g = ((hex >> 8) & 0xFF) / 255.0f;
    float b = (hex & 0xFF) / 255.0f;
    return Color(r, g, b, 1.0f);
}

Rect Rect::intersect(const Rect& o) const {
    float x1 = std::max(x, o.x);
    float y1 = std::max(y, o.y);
    float x2 = std::min(x + width, o.x + o.width);
    float y2 = std::min(y + height, o.y + o.height);
    if (x2 < x1 || y2 < y1) return Rect(0, 0, 0, 0);
    return Rect(x1, y1, x2 - x1, y2 - y1);
}

} // namespace VEOEngine
