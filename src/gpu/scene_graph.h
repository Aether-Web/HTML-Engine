#pragma once
#include "src/render/render_command.h"
#include "include/std/types.h"
#include <vector>
#include <memory>
#include <functional>

namespace VEOEngine {

struct SceneNode {
    Vec2 transform{0, 0};
    float opacity = 1.0f;
    bool visible = true;
    CommandList drawCommands;
    std::vector<std::unique_ptr<SceneNode>> children;
    
    SceneNode() = default;
    SceneNode(const SceneNode&) = delete;
    SceneNode& operator=(const SceneNode&) = delete;
};

class SceneGraph {
public:
    SceneGraph();
    
    SceneNode* root();
    SceneNode* addNode(SceneNode* parent = nullptr);
    void clear();
    void traverse(std::function<void(const SceneNode&, Vec2 offset, float opacity)> visitor) const;
    CommandList flatten() const;

private:
    std::unique_ptr<SceneNode> root_;
    void traverseNode(const SceneNode& node, Vec2 offset, float opacity,
                      std::function<void(const SceneNode&, Vec2, float)>& visitor) const;
};

} // namespace VEOEngine
