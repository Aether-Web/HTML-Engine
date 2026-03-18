#include "src/dom/document.h"

namespace VEOEngine {

Document::Document() : Node(NodeType::DOCUMENT) {}

std::shared_ptr<Element> Document::createElement(const std::string& tagName) {
    return std::make_shared<Element>(tagName);
}

std::shared_ptr<TextNode> Document::createTextNode(const std::string& content) {
    return std::make_shared<TextNode>(content);
}

std::shared_ptr<Element> Document::getElementById(const std::string& id) {
    if (documentElement) {
        if (documentElement->id == id) return documentElement;
        return documentElement->getElementById(id);
    }
    return nullptr;
}

std::shared_ptr<Element> Document::querySelector(const std::string& selector) {
    if (documentElement) {
        return documentElement->querySelector(selector);
    }
    return nullptr;
}

std::vector<std::shared_ptr<Element>> Document::querySelectorAll(const std::string& selector) {
    if (documentElement) {
        return documentElement->querySelectorAll(selector);
    }
    return {};
}

std::shared_ptr<Node> Document::cloneNode(bool deep) const {
    auto doc = std::make_shared<Document>();
    if (deep) {
        for (auto& child : children) {
            auto cloned = child->cloneNode(true);
            doc->appendChild(cloned);
        }
    }
    return doc;
}

} // namespace VEOEngine
