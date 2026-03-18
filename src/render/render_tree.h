#pragma once
#include "include/std/types.h"
#include "src/dom/node.h"
#include "src/layout/layout_engine.h"
#include <vector>
#include <memory>
#include <string>

namespace VEOEngine {

enum class RenderNodeType { Box, Text, Image, Container };

struct RenderNode {
    RenderNodeType type;
    Rect bounds;
    int zIndex = 0;
    bool visible = true;
    float opacity = 1.0f;
    std::vector<std::shared_ptr<RenderNode>> children;
    std::shared_ptr<Node> domNode;
    
    virtual ~RenderNode() = default;
};

struct RenderBox : public RenderNode {
    Color backgroundColor;
    Color borderColor;
    float borderWidth = 0;
    float cornerRadius = 0;
    RenderBox() { type = RenderNodeType::Box; }
};

struct RenderText : public RenderNode {
    std::string text;
    Color color;
    float fontSize = 16.0f;
    std::string fontFamily = "sans-serif";
    int fontWeight = 400;
    RenderText() { type = RenderNodeType::Text; }
};

struct RenderImage : public RenderNode {
    std::string source;
    RenderImage() { type = RenderNodeType::Image; }
};

struct RenderContainer : public RenderNode {
    RenderContainer() { type = RenderNodeType::Container; }
};

std::shared_ptr<RenderNode> buildRenderTree(const LayoutBox& layoutBox);

} // namespace VEOEngine
