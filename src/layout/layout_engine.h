#pragma once
#include "src/layout/box_model.h"
#include "src/dom/node.h"
#include "src/dom/document.h"
#include <vector>
#include <memory>

namespace VEOEngine {

enum class BoxType { Block, Inline, AnonymousBlock, InlineBlock };

struct LayoutBox {
    BoxDimensions dimensions;
    std::shared_ptr<Node> domNode;
    std::vector<LayoutBox> children;
    BoxType boxType;
    
    LayoutBox() : boxType(BoxType::Block) {}
    explicit LayoutBox(BoxType type) : boxType(type) {}
};

class LayoutEngine {
public:
    LayoutEngine(int viewportWidth, int viewportHeight);
    LayoutBox layout(const Document& doc);

private:
    int viewportWidth_;
    int viewportHeight_;
    
    LayoutBox buildLayoutTree(std::shared_ptr<Node> node);
    void calculateLayout(LayoutBox& box, const BoxDimensions& containingBlock);
    void calculateBlockLayout(LayoutBox& box, const BoxDimensions& containingBlock);
    void calculateInlineLayout(LayoutBox& box, const BoxDimensions& containingBlock);
    void calculateBlockWidth(LayoutBox& box, const BoxDimensions& containingBlock);
    void calculateBlockPosition(LayoutBox& box, const BoxDimensions& containingBlock);
    void layoutBlockChildren(LayoutBox& box);
    void calculateBlockHeight(LayoutBox& box);
    
    float resolveStyleValue(const StyleValue& val, float containerSize, float defaultVal = 0.0f) const;
};

} // namespace VEOEngine
