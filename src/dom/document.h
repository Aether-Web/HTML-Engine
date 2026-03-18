#pragma once
#include "src/dom/node.h"
#include <string>
#include <memory>

namespace VEOEngine {

class Document : public Node {
public:
    Document();
    
    std::shared_ptr<Element> head;
    std::shared_ptr<Element> body;
    std::string title;
    std::shared_ptr<Element> documentElement;
    
    std::shared_ptr<Element> createElement(const std::string& tagName);
    std::shared_ptr<TextNode> createTextNode(const std::string& content);
    
    std::shared_ptr<Element> getElementById(const std::string& id);
    std::shared_ptr<Element> querySelector(const std::string& selector);
    std::vector<std::shared_ptr<Element>> querySelectorAll(const std::string& selector);
    
    std::shared_ptr<Node> cloneNode(bool deep = false) const override;
};

} // namespace VEOEngine
