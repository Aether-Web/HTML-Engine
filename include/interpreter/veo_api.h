#pragma once
#include <string>
#include <vector>
#include <memory>

namespace VEOEngine {

class Node;
class Element;

class IVEODOMApi {
public:
    virtual ~IVEODOMApi() = default;
    virtual std::shared_ptr<Element> getElementById(const std::string& id) = 0;
    virtual std::vector<std::shared_ptr<Element>> querySelectorAll(const std::string& selector) = 0;
    virtual void setAttribute(const std::string& id, const std::string& attr, const std::string& value) = 0;
    virtual void setStyle(const std::string& id, const std::string& property, const std::string& value) = 0;
    virtual void setInnerHTML(const std::string& id, const std::string& html) = 0;
    virtual void log(const std::string& message) = 0;
};

} // namespace VEOEngine
