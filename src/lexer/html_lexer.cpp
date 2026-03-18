#include "src/lexer/html_lexer.h"
#include "include/utils/string_utils.h"
#include <algorithm>

namespace VEOEngine {

const std::vector<std::string> HtmlLexer::VOID_ELEMENTS = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

HtmlLexer::HtmlLexer(const std::string& html) : src_(html), pos_(0) {}

char HtmlLexer::peek(size_t offset) const {
    if (pos_ + offset >= src_.size()) return '\0';
    return src_[pos_ + offset];
}

char HtmlLexer::consume() {
    if (pos_ >= src_.size()) return '\0';
    return src_[pos_++];
}

bool HtmlLexer::match(const std::string& str) {
    if (pos_ + str.size() > src_.size()) return false;
    if (src_.substr(pos_, str.size()) == str) {
        pos_ += str.size();
        return true;
    }
    return false;
}

void HtmlLexer::skipWhitespace() {
    while (pos_ < src_.size() && std::isspace(static_cast<unsigned char>(src_[pos_]))) pos_++;
}

std::vector<Token> HtmlLexer::tokenize() {
    std::vector<Token> tokens;
    while (pos_ < src_.size()) {
        if (peek() == '<') {
            if (pos_ + 1 < src_.size()) {
                if (src_[pos_+1] == '!') {
                    if (pos_ + 3 < src_.size() && src_.substr(pos_+2, 2) == "--") {
                        tokens.push_back(readComment());
                    } else {
                        tokens.push_back(readDoctype());
                    }
                } else if (src_[pos_+1] == '/') {
                    tokens.push_back(readTag());
                } else {
                    Token t = readTag();
                    std::string lname = toLower(t.tagName);
                    tokens.push_back(t);
                    if (t.type == TokenType::OPEN_TAG && (lname == "script" || lname == "style")) {
                        std::string endTag = "</" + lname;
                        std::string raw = readRawContent(endTag);
                        Token ct;
                        ct.type = (lname == "script") ? TokenType::SCRIPT_CONTENT : TokenType::STYLE_CONTENT;
                        ct.tagName = lname;
                        ct.content = raw;
                        tokens.push_back(ct);
                        if (pos_ < src_.size() && src_[pos_] == '<') {
                            Token closeT = readTag();
                            tokens.push_back(closeT);
                        }
                    }
                }
            }
        } else {
            Token t = readText();
            if (!t.content.empty()) {
                tokens.push_back(t);
            }
        }
    }
    Token eof;
    eof.type = TokenType::END_OF_FILE;
    tokens.push_back(eof);
    return tokens;
}

Token HtmlLexer::readDoctype() {
    Token t;
    t.type = TokenType::DOCTYPE;
    pos_++; // '<'
    pos_++; // '!'
    while (pos_ < src_.size() && src_[pos_] != '>') {
        t.content += src_[pos_++];
    }
    if (pos_ < src_.size()) pos_++;
    return t;
}

Token HtmlLexer::readComment() {
    Token t;
    t.type = TokenType::COMMENT;
    pos_ += 4; // '<!--'
    while (pos_ < src_.size()) {
        if (pos_ + 2 < src_.size() && src_.substr(pos_, 3) == "-->") {
            pos_ += 3;
            break;
        }
        t.content += src_[pos_++];
    }
    return t;
}

Token HtmlLexer::readTag() {
    Token t;
    pos_++; // '<'
    
    bool isClose = false;
    if (peek() == '/') {
        isClose = true;
        pos_++;
    }
    
    while (pos_ < src_.size() && !std::isspace(static_cast<unsigned char>(src_[pos_])) && src_[pos_] != '>' && src_[pos_] != '/') {
        t.tagName += src_[pos_++];
    }
    t.tagName = toLower(t.tagName);
    
    if (isClose) {
        t.type = TokenType::CLOSE_TAG;
        while (pos_ < src_.size() && src_[pos_] != '>') pos_++;
        if (pos_ < src_.size()) pos_++;
        return t;
    }
    
    t.attributes = readAttributes();
    
    bool selfClose = false;
    if (pos_ < src_.size() && src_[pos_] == '/') {
        selfClose = true;
        pos_++;
    }
    if (pos_ < src_.size() && src_[pos_] == '>') {
        pos_++;
    }
    
    bool isVoid = std::find(VOID_ELEMENTS.begin(), VOID_ELEMENTS.end(), t.tagName) != VOID_ELEMENTS.end();
    
    if (selfClose || isVoid) {
        t.type = TokenType::SELF_CLOSING_TAG;
    } else {
        t.type = TokenType::OPEN_TAG;
    }
    
    return t;
}

std::map<std::string, std::string> HtmlLexer::readAttributes() {
    std::map<std::string, std::string> attrs;
    while (pos_ < src_.size() && src_[pos_] != '>' && !(src_[pos_] == '/' && pos_+1 < src_.size() && src_[pos_+1] == '>')) {
        skipWhitespace();
        if (pos_ >= src_.size() || src_[pos_] == '>' || src_[pos_] == '/') break;
        
        std::string name;
        while (pos_ < src_.size() && !std::isspace(static_cast<unsigned char>(src_[pos_])) && src_[pos_] != '=' && src_[pos_] != '>' && src_[pos_] != '/') {
            name += src_[pos_++];
        }
        name = toLower(name);
        if (name.empty()) { pos_++; continue; }
        
        skipWhitespace();
        if (pos_ < src_.size() && src_[pos_] == '=') {
            pos_++;
            skipWhitespace();
            attrs[name] = readAttributeValue();
        } else {
            attrs[name] = "";
        }
    }
    return attrs;
}

std::string HtmlLexer::readAttributeValue() {
    if (pos_ >= src_.size()) return "";
    
    if (src_[pos_] == '"' || src_[pos_] == '\'') {
        char quote = src_[pos_++];
        std::string val;
        while (pos_ < src_.size() && src_[pos_] != quote) {
            val += src_[pos_++];
        }
        if (pos_ < src_.size()) pos_++;
        return val;
    } else {
        std::string val;
        while (pos_ < src_.size() && !std::isspace(static_cast<unsigned char>(src_[pos_])) && src_[pos_] != '>') {
            val += src_[pos_++];
        }
        return val;
    }
}

Token HtmlLexer::readText() {
    Token t;
    t.type = TokenType::TEXT;
    while (pos_ < src_.size() && src_[pos_] != '<') {
        t.content += src_[pos_++];
    }
    return t;
}

std::string HtmlLexer::readRawContent(const std::string& endTag) {
    std::string content;
    std::string lowerEndTag = toLower(endTag);
    while (pos_ < src_.size()) {
        if (pos_ + lowerEndTag.size() <= src_.size()) {
            std::string ahead = toLower(src_.substr(pos_, lowerEndTag.size()));
            if (ahead == lowerEndTag) {
                break;
            }
        }
        content += src_[pos_++];
    }
    return content;
}

} // namespace VEOEngine
