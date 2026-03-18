#include "src/render/render_tree.h"
#include "src/render/render_command.h"

namespace VEOEngine {

std::shared_ptr<RenderNode> buildRenderTree(const LayoutBox& layoutBox) {
    if (!layoutBox.domNode) {
        auto container = std::make_shared<RenderContainer>();
        container->bounds = layoutBox.dimensions.content;
        for (const auto& child : layoutBox.children) {
            auto childNode = buildRenderTree(child);
            if (childNode) container->children.push_back(childNode);
        }
        return container;
    }
    
    if (layoutBox.domNode->isText()) {
        auto textNode = std::dynamic_pointer_cast<TextNode>(layoutBox.domNode);
        auto renderText = std::make_shared<RenderText>();
        renderText->bounds = layoutBox.dimensions.content;
        renderText->domNode = layoutBox.domNode;
        if (textNode) renderText->text = textNode->content;
        renderText->color = Color::Black();
        return renderText;
    }
    
    if (layoutBox.domNode->isElement()) {
        auto el = std::dynamic_pointer_cast<Element>(layoutBox.domNode);
        auto box = std::make_shared<RenderBox>();
        box->bounds = layoutBox.dimensions.borderBox();
        box->domNode = layoutBox.domNode;
        box->backgroundColor = el->style.backgroundColor;
        box->borderColor = el->style.border.color;
        box->borderWidth = el->style.border.width;
        box->opacity = el->style.opacity;
        box->zIndex = (int)el->style.zIndex;
        
        for (const auto& child : layoutBox.children) {
            auto childNode = buildRenderTree(child);
            if (childNode) box->children.push_back(childNode);
        }
        return box;
    }
    
    return nullptr;
}

CommandList RenderTreeFlattener::flatten(const RenderNode& root) {
    CommandList commands;
    visit(root, commands, 1.0f);
    return commands;
}

void RenderTreeFlattener::visit(const RenderNode& node, CommandList& commands, float parentOpacity) {
    if (!node.visible) return;
    float effectiveOpacity = parentOpacity * node.opacity;
    
    if (node.type == RenderNodeType::Box) {
        const auto& box = static_cast<const RenderBox&>(node);
        if (box.backgroundColor.a > 0.01f || box.borderWidth > 0) {
            DrawCommand cmd;
            cmd.type = CommandType::DRAW_RECT;
            cmd.bounds = box.bounds;
            Color bg = box.backgroundColor;
            bg.a *= effectiveOpacity;
            cmd.fillColor = bg;
            cmd.strokeColor = box.borderColor;
            cmd.strokeWidth = box.borderWidth;
            commands.push_back(cmd);
        }
    } else if (node.type == RenderNodeType::Text) {
        const auto& text = static_cast<const RenderText&>(node);
        if (!text.text.empty()) {
            DrawCommand cmd;
            cmd.type = CommandType::DRAW_TEXT;
            cmd.bounds = text.bounds;
            Color c = text.color;
            c.a *= effectiveOpacity;
            cmd.fillColor = c;
            cmd.text = text.text;
            cmd.fontSize = text.fontSize;
            cmd.fontFamily = text.fontFamily;
            cmd.fontWeight = text.fontWeight;
            commands.push_back(cmd);
        }
    } else if (node.type == RenderNodeType::Image) {
        const auto& img = static_cast<const RenderImage&>(node);
        DrawCommand cmd;
        cmd.type = CommandType::DRAW_IMAGE;
        cmd.bounds = img.bounds;
        cmd.imageSource = img.source;
        commands.push_back(cmd);
    }
    
    for (const auto& child : node.children) {
        if (child) visit(*child, commands, effectiveOpacity);
    }
}

} // namespace VEOEngine
