#pragma once
#include <string>
#include <vector>
#include <map>

namespace VEOEngine {

enum class TokenType {
    DOCTYPE,
    OPEN_TAG,
    CLOSE_TAG,
    SELF_CLOSING_TAG,
    TEXT,
    COMMENT,
    SCRIPT_CONTENT,
    STYLE_CONTENT,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::string content;
};

class HtmlLexer {
public:
    explicit HtmlLexer(const std::string& html);
    std::vector<Token> tokenize();

private:
    std::string src_;
    size_t pos_;
    
    void skipWhitespace();
    char peek(size_t offset = 0) const;
    char consume();
    bool match(const std::string& str);
    
    Token readDoctype();
    Token readComment();
    Token readTag();
    Token readText();
    std::string readRawContent(const std::string& endTag);
    std::map<std::string, std::string> readAttributes();
    std::string readAttributeValue();
    
    static const std::vector<std::string> VOID_ELEMENTS;
};

} // namespace VEOEngine
