#pragma once
#include "include/std/types.h"
#include <string>
#include <vector>

namespace VEOEngine {

enum class CommandType {
    CLEAR,
    DRAW_RECT,
    DRAW_ROUNDED_RECT,
    DRAW_TEXT,
    DRAW_IMAGE,
    DRAW_LINE,
    SET_CLIP,
    RESET_CLIP
};

struct DrawCommand {
    CommandType type = CommandType::DRAW_RECT;
    Rect bounds;
    Color fillColor;
    Color strokeColor;
    float strokeWidth = 0;
    float cornerRadius = 0;
    std::string text;
    float fontSize = 16;
    std::string fontFamily = "sans-serif";
    int fontWeight = 400;
    std::string imageSource;
    Rect clipRect;
};

using CommandList = std::vector<DrawCommand>;

class RenderNode;

class RenderTreeFlattener {
public:
    CommandList flatten(const RenderNode& root);
private:
    void visit(const RenderNode& node, CommandList& commands, float parentOpacity = 1.0f);
};

} // namespace VEOEngine
