#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
#include "src/dom/style.h"

namespace VEOEngine {

enum class NodeType { ELEMENT, TEXT, COMMENT, DOCUMENT };

class Node : public std::enable_shared_from_this<Node> {
public:
    NodeType nodeType;
    std::weak_ptr<Node> parent;
    std::vector<std::shared_ptr<Node>> children;
    
    explicit Node(NodeType type) : nodeType(type) {}
    virtual ~Node() = default;
    
    void appendChild(std::shared_ptr<Node> child);
    void removeChild(std::shared_ptr<Node> child);
    void insertBefore(std::shared_ptr<Node> newNode, std::shared_ptr<Node> refNode);
    virtual std::shared_ptr<Node> cloneNode(bool deep = false) const = 0;
    
    bool isElement() const { return nodeType == NodeType::ELEMENT; }
    bool isText() const { return nodeType == NodeType::TEXT; }
};

class TextNode : public Node {
public:
    std::string content;
    explicit TextNode(const std::string& text) : Node(NodeType::TEXT), content(text) {}
    std::shared_ptr<Node> cloneNode(bool deep = false) const override;
};

class CommentNode : public Node {
public:
    std::string content;
    explicit CommentNode(const std::string& text) : Node(NodeType::COMMENT), content(text) {}
    std::shared_ptr<Node> cloneNode(bool deep = false) const override;
};

class Element : public Node {
public:
    std::string tagName;
    std::string id;
    std::string className;
    std::map<std::string, std::string> attributes;
    Style style;
    
    explicit Element(const std::string& tag);
    
    std::string getAttribute(const std::string& name) const;
    void setAttribute(const std::string& name, const std::string& value);
    bool hasAttribute(const std::string& name) const;
    
    std::shared_ptr<Element> getElementById(const std::string& id);
    std::shared_ptr<Element> querySelector(const std::string& selector);
    std::vector<std::shared_ptr<Element>> querySelectorAll(const std::string& selector);
    std::vector<std::shared_ptr<Element>> getElementsByTagName(const std::string& tag);
    std::vector<std::shared_ptr<Element>> getElementsByClassName(const std::string& cls);
    
    std::shared_ptr<Node> cloneNode(bool deep = false) const override;
    
private:
    bool matchesSelector(const std::string& selector) const;
};

} // namespace VEOEngine
