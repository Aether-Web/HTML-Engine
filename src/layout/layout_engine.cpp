#include "src/layout/layout_engine.h"
#include "include/utils/string_utils.h"
#include <algorithm>
#include <variant>

namespace VEOEngine {

LayoutEngine::LayoutEngine(int viewportWidth, int viewportHeight)
    : viewportWidth_(viewportWidth), viewportHeight_(viewportHeight) {}

float LayoutEngine::resolveStyleValue(const StyleValue& val, float containerSize, float defaultVal) const {
    return std::visit([&](auto&& v) -> float {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, StyleValuePx>) return v.value;
        if constexpr (std::is_same_v<T, StyleValuePercent>) return containerSize * v.value / 100.0f;
        return defaultVal;
    }, val);
}

LayoutBox LayoutEngine::layout(const Document& doc) {
    BoxDimensions viewport;
    viewport.content = Rect(0, 0, (float)viewportWidth_, (float)viewportHeight_);
    
    if (doc.documentElement) {
        LayoutBox rootBox = buildLayoutTree(doc.documentElement);
        calculateLayout(rootBox, viewport);
        return rootBox;
    }
    
    LayoutBox root;
    root.dimensions.content = viewport.content;
    return root;
}

LayoutBox LayoutEngine::buildLayoutTree(std::shared_ptr<Node> node) {
    LayoutBox box;
    box.domNode = node;
    
    if (node->isText()) {
        box.boxType = BoxType::Inline;
        return box;
    }
    
    if (!node->isElement()) {
        box.boxType = BoxType::Block;
        return box;
    }
    
    auto el = std::dynamic_pointer_cast<Element>(node);
    const Style& style = el->style;
    
    switch (style.display) {
        case Display::Inline:
        case Display::InlineBlock:
            box.boxType = BoxType::Inline;
            break;
        case Display::None:
            box.boxType = BoxType::Block;
            return box;
        default:
            box.boxType = BoxType::Block;
    }
    
    for (auto& child : node->children) {
        if (child->nodeType == NodeType::COMMENT) continue;
        LayoutBox childBox = buildLayoutTree(child);
        box.children.push_back(childBox);
    }
    
    return box;
}

void LayoutEngine::calculateLayout(LayoutBox& box, const BoxDimensions& containingBlock) {
    if (box.boxType == BoxType::Inline) {
        calculateInlineLayout(box, containingBlock);
    } else {
        calculateBlockLayout(box, containingBlock);
    }
}

void LayoutEngine::calculateBlockLayout(LayoutBox& box, const BoxDimensions& containingBlock) {
    calculateBlockWidth(box, containingBlock);
    calculateBlockPosition(box, containingBlock);
    layoutBlockChildren(box);
    calculateBlockHeight(box);
}

void LayoutEngine::calculateInlineLayout(LayoutBox& box, const BoxDimensions& containingBlock) {
    if (box.domNode && box.domNode->isText()) {
        auto textNode = std::dynamic_pointer_cast<TextNode>(box.domNode);
        if (textNode) {
            box.dimensions.content.width = std::min((float)(textNode->content.size() * 8), containingBlock.content.width);
            box.dimensions.content.height = 16.0f;
        }
        return;
    }
    calculateBlockLayout(box, containingBlock);
}

void LayoutEngine::calculateBlockWidth(LayoutBox& box, const BoxDimensions& containingBlock) {
    if (!box.domNode || !box.domNode->isElement()) {
        box.dimensions.content.width = containingBlock.content.width;
        return;
    }
    
    auto el = std::dynamic_pointer_cast<Element>(box.domNode);
    const Style& style = el->style;
    float containerWidth = containingBlock.content.width;
    
    float marginLeft = resolveStyleValue(style.margin.left, containerWidth);
    float marginRight = resolveStyleValue(style.margin.right, containerWidth);
    float paddingLeft = resolveStyleValue(style.padding.left, containerWidth);
    float paddingRight = resolveStyleValue(style.padding.right, containerWidth);
    float borderLeft = style.border.width;
    float borderRight = style.border.width;
    
    box.dimensions.margin.left = marginLeft;
    box.dimensions.margin.right = marginRight;
    box.dimensions.padding.left = paddingLeft;
    box.dimensions.padding.right = paddingRight;
    box.dimensions.border.left = borderLeft;
    box.dimensions.border.right = borderRight;
    
    bool widthAuto = std::holds_alternative<StyleValueAuto>(style.width);
    if (widthAuto) {
        float usedWidth = marginLeft + marginRight + paddingLeft + paddingRight + borderLeft + borderRight;
        box.dimensions.content.width = containerWidth - usedWidth;
    } else {
        float w = resolveStyleValue(style.width, containerWidth);
        box.dimensions.content.width = w;
    }
    
    if (box.dimensions.content.width < 0) box.dimensions.content.width = 0;
}

void LayoutEngine::calculateBlockPosition(LayoutBox& box, const BoxDimensions& containingBlock) {
    if (!box.domNode || !box.domNode->isElement()) {
        box.dimensions.content.x = containingBlock.content.x;
        box.dimensions.content.y = containingBlock.content.y;
        return;
    }
    
    auto el = std::dynamic_pointer_cast<Element>(box.domNode);
    const Style& style = el->style;
    float containerWidth = containingBlock.content.width;
    float containerHeight = containingBlock.content.height;
    
    float marginTop = resolveStyleValue(style.margin.top, containerHeight);
    float marginBottom = resolveStyleValue(style.margin.bottom, containerHeight);
    float paddingTop = resolveStyleValue(style.padding.top, containerWidth);
    float paddingBottom = resolveStyleValue(style.padding.bottom, containerWidth);
    float borderTop = style.border.width;
    float borderBottom = style.border.width;
    
    box.dimensions.margin.top = marginTop;
    box.dimensions.margin.bottom = marginBottom;
    box.dimensions.padding.top = paddingTop;
    box.dimensions.padding.bottom = paddingBottom;
    box.dimensions.border.top = borderTop;
    box.dimensions.border.bottom = borderBottom;
    
    box.dimensions.content.x = containingBlock.content.x
        + box.dimensions.margin.left
        + box.dimensions.border.left
        + box.dimensions.padding.left;
    
    box.dimensions.content.y = containingBlock.content.y
        + containingBlock.content.height
        + box.dimensions.margin.top
        + box.dimensions.border.top
        + box.dimensions.padding.top;
}

void LayoutEngine::layoutBlockChildren(LayoutBox& box) {
    for (auto& child : box.children) {
        calculateLayout(child, box.dimensions);
        box.dimensions.content.height += child.dimensions.marginBox().height;
    }
}

void LayoutEngine::calculateBlockHeight(LayoutBox& box) {
    if (!box.domNode || !box.domNode->isElement()) return;
    
    auto el = std::dynamic_pointer_cast<Element>(box.domNode);
    const Style& style = el->style;
    
    if (!std::holds_alternative<StyleValueAuto>(style.height)) {
        float h = resolveStyleValue(style.height, (float)viewportHeight_);
        if (h > 0) box.dimensions.content.height = h;
    }
    
    if (box.dimensions.content.height == 0) {
        std::string tag = el->tagName;
        if (tag == "h1") box.dimensions.content.height = 40.0f;
        else if (tag == "h2") box.dimensions.content.height = 32.0f;
        else if (tag == "h3") box.dimensions.content.height = 28.0f;
        else if (tag == "h4" || tag == "h5" || tag == "h6") box.dimensions.content.height = 24.0f;
        else if (tag == "p") box.dimensions.content.height = 20.0f;
        else if (tag == "li") box.dimensions.content.height = 20.0f;
    }
}

} // namespace VEOEngine
