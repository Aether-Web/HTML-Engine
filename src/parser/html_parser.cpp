#include "src/parser/html_parser.h"
#include "include/utils/string_utils.h"
#include <algorithm>
#include <iostream>

namespace VEOEngine {

const std::vector<std::string> HtmlParser::VOID_ELEMENTS = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

HtmlParser::HtmlParser(const std::vector<Token>& tokens) : tokens_(tokens), pos_(0) {}

const Token& HtmlParser::peek() const {
    static Token eof{TokenType::END_OF_FILE, "", {}, ""};
    if (pos_ >= tokens_.size()) return eof;
    return tokens_[pos_];
}

const Token& HtmlParser::consume() {
    static Token eof{TokenType::END_OF_FILE, "", {}, ""};
    if (pos_ >= tokens_.size()) return eof;
    return tokens_[pos_++];
}

bool HtmlParser::shouldAutoClose(const std::string& openTag, const std::string& newTag) const {
    if (openTag == "li" && newTag == "li") return true;
    if (openTag == "p" && (newTag == "p" || newTag == "div" || newTag == "h1" || newTag == "h2" ||
        newTag == "h3" || newTag == "h4" || newTag == "h5" || newTag == "h6" ||
        newTag == "ul" || newTag == "ol" || newTag == "table" || newTag == "blockquote")) return true;
    if ((openTag == "td" || openTag == "th") && (newTag == "td" || newTag == "th" || newTag == "tr")) return true;
    if (openTag == "tr" && newTag == "tr") return true;
    if (openTag == "option" && newTag == "option") return true;
    return false;
}

std::unique_ptr<Document> HtmlParser::parse() {
    auto doc = std::make_unique<Document>();
    parseDocument(*doc);
    return doc;
}

void HtmlParser::parseDocument(Document& doc) {
    std::stack<std::shared_ptr<Element>> elementStack;
    std::vector<std::shared_ptr<Node>> parentStack;
    
    auto docPtr = std::shared_ptr<Document>(&doc, [](Document*){});
    parentStack.push_back(docPtr);
    
    while (pos_ < tokens_.size()) {
        const Token& tok = peek();
        
        if (tok.type == TokenType::END_OF_FILE) {
            consume();
            break;
        }
        
        if (tok.type == TokenType::DOCTYPE) {
            consume();
            continue;
        }
        
        if (tok.type == TokenType::COMMENT) {
            auto comment = std::make_shared<CommentNode>(tok.content);
            parentStack.back()->appendChild(comment);
            consume();
            continue;
        }
        
        if (tok.type == TokenType::TEXT) {
            std::string content = tok.content;
            bool allWhitespace = true;
            for (char c : content) {
                if (!std::isspace(static_cast<unsigned char>(c))) { allWhitespace = false; break; }
            }
            if (!allWhitespace) {
                auto textNode = std::make_shared<TextNode>(content);
                parentStack.back()->appendChild(textNode);
            }
            consume();
            continue;
        }
        
        if (tok.type == TokenType::SCRIPT_CONTENT || tok.type == TokenType::STYLE_CONTENT) {
            auto textNode = std::make_shared<TextNode>(tok.content);
            parentStack.back()->appendChild(textNode);
            consume();
            continue;
        }
        
        if (tok.type == TokenType::OPEN_TAG) {
            consume();
            std::string tagName = tok.tagName;
            
            if (!elementStack.empty()) {
                while (!elementStack.empty() && shouldAutoClose(elementStack.top()->tagName, tagName)) {
                    elementStack.pop();
                    if (parentStack.size() > 1) parentStack.pop_back();
                }
            }
            
            auto el = std::make_shared<Element>(tagName);
            for (auto& [k, v] : tok.attributes) {
                el->setAttribute(k, v);
            }
            
            if (tagName == "html") {
                doc.documentElement = el;
            } else if (tagName == "head") {
                doc.head = el;
            } else if (tagName == "body") {
                doc.body = el;
            }
            
            parentStack.back()->appendChild(el);
            
            bool isVoid = std::find(VOID_ELEMENTS.begin(), VOID_ELEMENTS.end(), tagName) != VOID_ELEMENTS.end();
            if (!isVoid) {
                parentStack.push_back(el);
                elementStack.push(el);
            }
            continue;
        }
        
        if (tok.type == TokenType::SELF_CLOSING_TAG) {
            consume();
            auto el = std::make_shared<Element>(tok.tagName);
            for (auto& [k, v] : tok.attributes) {
                el->setAttribute(k, v);
            }
            parentStack.back()->appendChild(el);
            continue;
        }
        
        if (tok.type == TokenType::CLOSE_TAG) {
            consume();
            std::string tagName = tok.tagName;
            
            while (!elementStack.empty()) {
                if (elementStack.top()->tagName == tagName) {
                    elementStack.pop();
                    if (parentStack.size() > 1) parentStack.pop_back();
                    break;
                }
                elementStack.pop();
                if (parentStack.size() > 1) parentStack.pop_back();
            }
            continue;
        }
        
        consume();
    }
    
    if (doc.head) {
        auto titleEls = doc.head->getElementsByTagName("title");
        if (!titleEls.empty() && !titleEls[0]->children.empty()) {
            auto textChild = std::dynamic_pointer_cast<TextNode>(titleEls[0]->children[0]);
            if (textChild) {
                doc.title = trim(textChild->content);
            }
        }
    }
}

} // namespace VEOEngine
