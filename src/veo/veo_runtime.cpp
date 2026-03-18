#include "src/veo/veo_runtime.h"
#include "src/dom/document.h"
#include "include/utils/string_utils.h"
#include <iostream>

namespace VEOEngine {

VEODOMApi::VEODOMApi(Document& doc) : doc_(doc) {}

std::shared_ptr<Element> VEODOMApi::getElementById(const std::string& id) {
    return doc_.getElementById(id);
}

std::vector<std::shared_ptr<Element>> VEODOMApi::querySelectorAll(const std::string& selector) {
    return doc_.querySelectorAll(selector);
}

void VEODOMApi::setAttribute(const std::string& id, const std::string& attr, const std::string& value) {
    auto el = doc_.getElementById(id);
    if (el) el->setAttribute(attr, value);
}

void VEODOMApi::setStyle(const std::string& id, const std::string& property, const std::string& value) {
    auto el = doc_.getElementById(id);
    if (el) el->style.applyProperty(property, value);
}

void VEODOMApi::setInnerHTML(const std::string& id, const std::string& html) {
    auto el = doc_.getElementById(id);
    if (!el) return;
    el->children.clear();
    auto textNode = std::make_shared<TextNode>(html);
    el->appendChild(textNode);
}

void VEODOMApi::log(const std::string& message) {
    std::cout << "[VEO] " << message << std::endl;
}

std::vector<std::string> VEORuntime::extractVEOScripts(const Document& doc) {
    std::vector<std::string> scripts;
    if (!doc.documentElement) return scripts;
    
    auto scriptEls = doc.documentElement->getElementsByTagName("script");
    for (auto& el : scriptEls) {
        std::string type = el->getAttribute("type");
        if (toLower(trim(type)) == "veo") {
            std::string content;
            for (auto& child : el->children) {
                if (child->isText()) {
                    auto textNode = std::dynamic_pointer_cast<TextNode>(child);
                    if (textNode) content += textNode->content;
                }
            }
            if (!content.empty()) scripts.push_back(content);
        }
    }
    return scripts;
}

void VEORuntime::execute(const std::string& script, Document& /*doc*/, VEODOMApi& api) {
    auto lines = split(script, '\n');
    for (auto& line : lines) {
        std::string trimmed = trim(line);
        if (!trimmed.empty()) {
            executeLine(trimmed, api);
        }
    }
}

void VEORuntime::executeLine(const std::string& line, VEODOMApi& api) {
    size_t parenOpen = line.find('(');
    size_t parenClose = line.rfind(')');
    if (parenOpen == std::string::npos) return;
    
    std::string cmd = trim(line.substr(0, parenOpen));
    std::string args;
    if (parenClose != std::string::npos && parenClose > parenOpen) {
        args = trim(line.substr(parenOpen + 1, parenClose - parenOpen - 1));
    }
    
    if (cmd == "log") {
        api.log(args);
    } else if (cmd == "set") {
        size_t commaPos = args.find(',');
        if (commaPos == std::string::npos) return;
        std::string target = trim(args.substr(0, commaPos));
        std::string value = trim(args.substr(commaPos + 1));
        
        size_t dotPos = target.find('.');
        if (dotPos != std::string::npos && !target.empty() && target[0] == '#') {
            std::string id = target.substr(1, dotPos - 1);
            std::string property = target.substr(dotPos + 1);
            api.setStyle(id, property, value);
        } else if (!target.empty() && target[0] == '#') {
            std::string id = target.substr(1);
            api.setAttribute(id, "data-value", value);
        }
    } else if (cmd == "append") {
        size_t commaPos = args.find(',');
        if (commaPos == std::string::npos) return;
        std::string target = trim(args.substr(0, commaPos));
        std::string html = trim(args.substr(commaPos + 1));
        if (!target.empty() && target[0] == '#') {
            std::string id = target.substr(1);
            api.log("append to #" + id + ": " + html);
        }
    }
}

std::string VEORuntime::parseTarget(const std::string& args) {
    size_t commaPos = args.find(',');
    if (commaPos != std::string::npos) return trim(args.substr(0, commaPos));
    return args;
}

} // namespace VEOEngine
