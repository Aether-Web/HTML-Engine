#include "src/gpu/scene_graph.h"

namespace VEOEngine {

SceneGraph::SceneGraph() : root_(std::make_unique<SceneNode>()) {}

SceneNode* SceneGraph::root() { return root_.get(); }

SceneNode* SceneGraph::addNode(SceneNode* parent) {
    auto node = std::make_unique<SceneNode>();
    SceneNode* ptr = node.get();
    if (parent) {
        parent->children.push_back(std::move(node));
    } else {
        root_->children.push_back(std::move(node));
    }
    return ptr;
}

void SceneGraph::clear() {
    root_ = std::make_unique<SceneNode>();
}

void SceneGraph::traverse(std::function<void(const SceneNode&, Vec2 offset, float opacity)> visitor) const {
    traverseNode(*root_, Vec2{0,0}, 1.0f, visitor);
}

void SceneGraph::traverseNode(const SceneNode& node, Vec2 offset, float opacity,
                               std::function<void(const SceneNode&, Vec2, float)>& visitor) const {
    if (!node.visible) return;
    Vec2 newOffset = offset + node.transform;
    float newOpacity = opacity * node.opacity;
    visitor(node, newOffset, newOpacity);
    for (const auto& child : node.children) {
        traverseNode(*child, newOffset, newOpacity, visitor);
    }
}

CommandList SceneGraph::flatten() const {
    CommandList result;
    traverse([&](const SceneNode& node, Vec2 offset, float /*opacity*/) {
        for (auto& cmd : node.drawCommands) {
            DrawCommand adjusted = cmd;
            adjusted.bounds.x += offset.x;
            adjusted.bounds.y += offset.y;
            result.push_back(adjusted);
        }
    });
    return result;
}

} // namespace VEOEngine
