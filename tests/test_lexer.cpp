#include "src/lexer/html_lexer.h"
#include <iostream>
#include <cassert>
#include <string>

using namespace VEOEngine;

void test_basic_tag() {
    HtmlLexer lexer("<div>Hello</div>");
    auto tokens = lexer.tokenize();
    
    bool foundOpen = false, foundText = false, foundClose = false;
    for (auto& t : tokens) {
        if (t.type == TokenType::OPEN_TAG && t.tagName == "div") foundOpen = true;
        if (t.type == TokenType::TEXT && t.content == "Hello") foundText = true;
        if (t.type == TokenType::CLOSE_TAG && t.tagName == "div") foundClose = true;
    }
    assert(foundOpen && "Should find open div tag");
    assert(foundText && "Should find text content");
    assert(foundClose && "Should find close div tag");
    std::cout << "test_basic_tag: PASSED\n";
}

void test_attributes() {
    HtmlLexer lexer("<div id=\"myDiv\" class=\"container\"></div>");
    auto tokens = lexer.tokenize();
    
    bool found = false;
    for (auto& t : tokens) {
        if (t.type == TokenType::OPEN_TAG && t.tagName == "div") {
            assert(t.attributes.count("id") && t.attributes.at("id") == "myDiv");
            assert(t.attributes.count("class") && t.attributes.at("class") == "container");
            found = true;
        }
    }
    assert(found && "Should find div with attributes");
    std::cout << "test_attributes: PASSED\n";
}

void test_void_elements() {
    HtmlLexer lexer("<br><img src=\"test.png\"><input type=\"text\">");
    auto tokens = lexer.tokenize();
    
    int selfClosingCount = 0;
    for (auto& t : tokens) {
        if (t.type == TokenType::SELF_CLOSING_TAG) selfClosingCount++;
    }
    assert(selfClosingCount == 3 && "Should find 3 self-closing/void elements");
    std::cout << "test_void_elements: PASSED\n";
}

void test_doctype() {
    HtmlLexer lexer("<!DOCTYPE html><html></html>");
    auto tokens = lexer.tokenize();
    
    bool foundDoctype = false;
    for (auto& t : tokens) {
        if (t.type == TokenType::DOCTYPE) foundDoctype = true;
    }
    assert(foundDoctype && "Should find DOCTYPE");
    std::cout << "test_doctype: PASSED\n";
}

void test_comment() {
    HtmlLexer lexer("<!-- This is a comment --><div></div>");
    auto tokens = lexer.tokenize();
    
    bool foundComment = false;
    for (auto& t : tokens) {
        if (t.type == TokenType::COMMENT) {
            foundComment = true;
            assert(t.content.find("This is a comment") != std::string::npos);
        }
    }
    assert(foundComment && "Should find comment");
    std::cout << "test_comment: PASSED\n";
}

void test_script_content() {
    HtmlLexer lexer("<script>var x = 1;</script>");
    auto tokens = lexer.tokenize();
    
    bool foundScript = false;
    for (auto& t : tokens) {
        if (t.type == TokenType::SCRIPT_CONTENT) {
            foundScript = true;
            assert(t.content.find("var x = 1;") != std::string::npos);
        }
    }
    assert(foundScript && "Should find script content");
    std::cout << "test_script_content: PASSED\n";
}

void test_nested_tags() {
    HtmlLexer lexer("<ul><li>Item 1</li><li>Item 2</li></ul>");
    auto tokens = lexer.tokenize();
    
    int openLi = 0, closeLi = 0;
    for (auto& t : tokens) {
        if (t.type == TokenType::OPEN_TAG && t.tagName == "li") openLi++;
        if (t.type == TokenType::CLOSE_TAG && t.tagName == "li") closeLi++;
    }
    assert(openLi == 2 && "Should find 2 open li tags");
    assert(closeLi == 2 && "Should find 2 close li tags");
    std::cout << "test_nested_tags: PASSED\n";
}

int main() {
    std::cout << "=== Lexer Tests ===" << std::endl;
    test_basic_tag();
    test_attributes();
    test_void_elements();
    test_doctype();
    test_comment();
    test_script_content();
    test_nested_tags();
    std::cout << "All lexer tests passed!" << std::endl;
    return 0;
}
