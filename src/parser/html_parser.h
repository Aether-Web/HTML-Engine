#pragma once
#include "src/lexer/html_lexer.h"
#include "src/dom/document.h"
#include <vector>
#include <memory>
#include <stack>

namespace VEOEngine {

class HtmlParser {
public:
    explicit HtmlParser(const std::vector<Token>& tokens);
    std::unique_ptr<Document> parse();

private:
    const std::vector<Token>& tokens_;
    size_t pos_;
    
    const Token& peek() const;
    const Token& consume();
    
    void parseDocument(Document& doc);
    bool shouldAutoClose(const std::string& openTag, const std::string& newTag) const;
    
    static const std::vector<std::string> VOID_ELEMENTS;
};

} // namespace VEOEngine
