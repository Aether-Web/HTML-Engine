#pragma once
#include "include/std/types.h"

namespace VEOEngine {

struct EdgeSizes {
    float top = 0, right = 0, bottom = 0, left = 0;
};

struct BoxDimensions {
    Rect content;
    EdgeSizes padding;
    EdgeSizes border;
    EdgeSizes margin;
    
    Rect paddingBox() const {
        return Rect(
            content.x - padding.left,
            content.y - padding.top,
            content.width + padding.left + padding.right,
            content.height + padding.top + padding.bottom
        );
    }
    
    Rect borderBox() const {
        Rect pb = paddingBox();
        return Rect(
            pb.x - border.left,
            pb.y - border.top,
            pb.width + border.left + border.right,
            pb.height + border.top + border.bottom
        );
    }
    
    Rect marginBox() const {
        Rect bb = borderBox();
        return Rect(
            bb.x - margin.left,
            bb.y - margin.top,
            bb.width + margin.left + margin.right,
            bb.height + margin.top + margin.bottom
        );
    }
};

} // namespace VEOEngine
