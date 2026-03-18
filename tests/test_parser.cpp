#include "src/lexer/html_lexer.h"
#include "src/parser/html_parser.h"
#include "src/dom/document.h"
#include <iostream>
#include <cassert>
#include <memory>

using namespace VEOEngine;

std::unique_ptr<Document> parseHTML(const std::string& html) {
    HtmlLexer lexer(html);
    auto tokens = lexer.tokenize();
    HtmlParser parser(tokens);
    return parser.parse();
}

void test_basic_structure() {
    auto doc = parseHTML("<!DOCTYPE html><html><head><title>Test</title></head><body><p>Hello</p></body></html>");
    
    assert(doc != nullptr);
    assert(doc->documentElement != nullptr && "Should have html element");
    assert(doc->head != nullptr && "Should have head element");
    assert(doc->body != nullptr && "Should have body element");
    assert(doc->title == "Test" && "Should extract title");
    std::cout << "test_basic_structure: PASSED\n";
}

void test_getElementById() {
    auto doc = parseHTML("<html><body><div id=\"myDiv\">Content</div></body></html>");
    
    auto el = doc->getElementById("myDiv");
    assert(el != nullptr && "Should find element by id");
    assert(el->tagName == "div" && "Should be a div");
    std::cout << "test_getElementById: PASSED\n";
}

void test_querySelector() {
    auto doc = parseHTML("<html><body><div class=\"box\">Content</div></body></html>");
    
    auto el = doc->querySelector(".box");
    assert(el != nullptr && "Should find element by class");
    assert(el->tagName == "div" && "Should be a div");
    std::cout << "test_querySelector: PASSED\n";
}

void test_nested_elements() {
    auto doc = parseHTML("<html><body><ul><li>A</li><li>B</li><li>C</li></ul></body></html>");
    
    assert(doc->body != nullptr);
    auto ulEls = doc->body->getElementsByTagName("ul");
    assert(!ulEls.empty() && "Should find ul");
    auto liEls = ulEls[0]->getElementsByTagName("li");
    assert(liEls.size() == 3 && "Should find 3 li elements");
    std::cout << "test_nested_elements: PASSED\n";
}

void test_attributes_preserved() {
    auto doc = parseHTML("<html><body><a href=\"http://example.com\" target=\"_blank\">Link</a></body></html>");
    
    auto links = doc->body->getElementsByTagName("a");
    assert(!links.empty() && "Should find anchor");
    assert(links[0]->getAttribute("href") == "http://example.com" && "Should preserve href");
    assert(links[0]->getAttribute("target") == "_blank" && "Should preserve target");
    std::cout << "test_attributes_preserved: PASSED\n";
}

void test_inline_style_parsed() {
    auto doc = parseHTML("<html><body><div style=\"color: red; background-color: blue;\">Styled</div></body></html>");
    
    auto divs = doc->body->getElementsByTagName("div");
    assert(!divs.empty() && "Should find div");
    const Style& style = divs[0]->style;
    assert(style.color.r > 0.9f && style.color.g < 0.1f && "Color should be red");
    assert(style.backgroundColor.b > 0.9f && "Background should be blue");
    std::cout << "test_inline_style_parsed: PASSED\n";
}

void test_text_content() {
    auto doc = parseHTML("<html><body><p>Hello World</p></body></html>");
    
    auto paras = doc->body->getElementsByTagName("p");
    assert(!paras.empty() && "Should find paragraph");
    assert(!paras[0]->children.empty() && "Paragraph should have children");
    auto textNode = std::dynamic_pointer_cast<TextNode>(paras[0]->children[0]);
    assert(textNode != nullptr && "Should be text node");
    assert(textNode->content == "Hello World" && "Should have correct text");
    std::cout << "test_text_content: PASSED\n";
}

int main() {
    std::cout << "=== Parser Tests ===" << std::endl;
    test_basic_structure();
    test_getElementById();
    test_querySelector();
    test_nested_elements();
    test_attributes_preserved();
    test_inline_style_parsed();
    test_text_content();
    std::cout << "All parser tests passed!" << std::endl;
    return 0;
}
