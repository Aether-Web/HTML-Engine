#include "src/dom/node.h"
#include "include/utils/string_utils.h"
#include <algorithm>

namespace VEOEngine {

void Node::appendChild(std::shared_ptr<Node> child) {
    child->parent = shared_from_this();
    children.push_back(child);
}

void Node::removeChild(std::shared_ptr<Node> child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->parent.reset();
        children.erase(it);
    }
}

void Node::insertBefore(std::shared_ptr<Node> newNode, std::shared_ptr<Node> refNode) {
    auto it = std::find(children.begin(), children.end(), refNode);
    newNode->parent = shared_from_this();
    if (it != children.end()) {
        children.insert(it, newNode);
    } else {
        children.push_back(newNode);
    }
}

std::shared_ptr<Node> TextNode::cloneNode(bool /*deep*/) const {
    return std::make_shared<TextNode>(content);
}

std::shared_ptr<Node> CommentNode::cloneNode(bool /*deep*/) const {
    return std::make_shared<CommentNode>(content);
}

Element::Element(const std::string& tag) : Node(NodeType::ELEMENT), tagName(toLower(tag)) {}

std::string Element::getAttribute(const std::string& name) const {
    auto it = attributes.find(name);
    return (it != attributes.end()) ? it->second : "";
}

void Element::setAttribute(const std::string& name, const std::string& value) {
    attributes[name] = value;
    if (name == "id") id = value;
    else if (name == "class") className = value;
    else if (name == "style") style = Style::parse(value);
}

bool Element::hasAttribute(const std::string& name) const {
    return attributes.count(name) > 0;
}

bool Element::matchesSelector(const std::string& selector) const {
    if (selector.empty()) return false;
    std::string sel = trim(selector);
    
    if (sel[0] == '#') {
        return id == sel.substr(1);
    }
    if (sel[0] == '.') {
        std::string cls = sel.substr(1);
        auto classes = split(className, ' ');
        return std::find(classes.begin(), classes.end(), cls) != classes.end();
    }
    size_t hashPos = sel.find('#');
    size_t dotPos = sel.find('.');
    if (hashPos != std::string::npos) {
        std::string tag = sel.substr(0, hashPos);
        std::string eid = sel.substr(hashPos+1);
        return (tag.empty() || tagName == tag) && id == eid;
    }
    if (dotPos != std::string::npos) {
        std::string tag = sel.substr(0, dotPos);
        std::string cls = sel.substr(dotPos+1);
        auto classes = split(className, ' ');
        bool classMatch = std::find(classes.begin(), classes.end(), cls) != classes.end();
        return (tag.empty() || tagName == tag) && classMatch;
    }
    return tagName == sel;
}

std::shared_ptr<Element> Element::getElementById(const std::string& eid) {
    for (auto& child : children) {
        if (child->isElement()) {
            auto el = std::dynamic_pointer_cast<Element>(child);
            if (el->id == eid) return el;
            auto found = el->getElementById(eid);
            if (found) return found;
        }
    }
    return nullptr;
}

std::shared_ptr<Element> Element::querySelector(const std::string& selector) {
    for (auto& child : children) {
        if (child->isElement()) {
            auto el = std::dynamic_pointer_cast<Element>(child);
            if (el->matchesSelector(selector)) return el;
            auto found = el->querySelector(selector);
            if (found) return found;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Element>> Element::querySelectorAll(const std::string& selector) {
    std::vector<std::shared_ptr<Element>> result;
    for (auto& child : children) {
        if (child->isElement()) {
            auto el = std::dynamic_pointer_cast<Element>(child);
            if (el->matchesSelector(selector)) result.push_back(el);
            auto sub = el->querySelectorAll(selector);
            result.insert(result.end(), sub.begin(), sub.end());
        }
    }
    return result;
}

std::vector<std::shared_ptr<Element>> Element::getElementsByTagName(const std::string& tag) {
    std::vector<std::shared_ptr<Element>> result;
    for (auto& child : children) {
        if (child->isElement()) {
            auto el = std::dynamic_pointer_cast<Element>(child);
            if (el->tagName == tag) result.push_back(el);
            auto sub = el->getElementsByTagName(tag);
            result.insert(result.end(), sub.begin(), sub.end());
        }
    }
    return result;
}

std::vector<std::shared_ptr<Element>> Element::getElementsByClassName(const std::string& cls) {
    std::vector<std::shared_ptr<Element>> result;
    for (auto& child : children) {
        if (child->isElement()) {
            auto el = std::dynamic_pointer_cast<Element>(child);
            auto classes = split(el->className, ' ');
            if (std::find(classes.begin(), classes.end(), cls) != classes.end()) {
                result.push_back(el);
            }
            auto sub = el->getElementsByClassName(cls);
            result.insert(result.end(), sub.begin(), sub.end());
        }
    }
    return result;
}

std::shared_ptr<Node> Element::cloneNode(bool deep) const {
    auto el = std::make_shared<Element>(tagName);
    el->id = id;
    el->className = className;
    el->attributes = attributes;
    el->style = style;
    if (deep) {
        for (auto& child : children) {
            auto cloned = child->cloneNode(true);
            el->appendChild(cloned);
        }
    }
    return el;
}

} // namespace VEOEngine
