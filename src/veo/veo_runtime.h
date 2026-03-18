#pragma once
#include <string>
#include <vector>
#include <memory>
#include "include/interpreter/veo_api.h"

namespace VEOEngine {

class Document;

class VEODOMApi : public IVEODOMApi {
public:
    explicit VEODOMApi(Document& doc);
    
    std::shared_ptr<Element> getElementById(const std::string& id) override;
    std::vector<std::shared_ptr<Element>> querySelectorAll(const std::string& selector) override;
    void setAttribute(const std::string& id, const std::string& attr, const std::string& value) override;
    void setStyle(const std::string& id, const std::string& property, const std::string& value) override;
    void setInnerHTML(const std::string& id, const std::string& html) override;
    void log(const std::string& message) override;

private:
    Document& doc_;
};

class VEORuntime {
public:
    VEORuntime() = default;
    
    std::vector<std::string> extractVEOScripts(const Document& doc);
    void execute(const std::string& script, Document& doc, VEODOMApi& api);

private:
    void executeLine(const std::string& line, VEODOMApi& api);
    std::string parseTarget(const std::string& args);
};

} // namespace VEOEngine
