#include "src/lexer/html_lexer.h"
#include "src/parser/html_parser.h"
#include "src/layout/layout_engine.h"
#include "src/render/render_tree.h"
#include "src/render/render_command.h"
#include "src/gpu/gpu_renderer.h"
#include <iostream>
#include <cassert>
#include <cmath>
#include <functional>

using namespace VEOEngine;

std::unique_ptr<Document> parseHTML(const std::string& html) {
    HtmlLexer lexer(html);
    auto tokens = lexer.tokenize();
    HtmlParser parser(tokens);
    return parser.parse();
}

void test_layout_viewport() {
    auto doc = parseHTML("<html><body></body></html>");
    LayoutEngine engine(800, 600);
    auto root = engine.layout(*doc);
    
    assert(root.dimensions.content.width > 0 && "Root should have width");
    std::cout << "test_layout_viewport: PASSED (root width=" << root.dimensions.content.width << ")\n";
}

void test_layout_block_width() {
    auto doc = parseHTML("<html><body><div style=\"width: 200px;\">Box</div></body></html>");
    LayoutEngine engine(800, 600);
    auto root = engine.layout(*doc);
    
    bool found = false;
    std::function<void(const LayoutBox&)> search = [&](const LayoutBox& box) {
        if (box.domNode && box.domNode->isElement()) {
            auto el = std::dynamic_pointer_cast<Element>(box.domNode);
            if (el && el->tagName == "div") {
                if (std::abs(box.dimensions.content.width - 200.0f) < 1.0f) {
                    found = true;
                }
            }
        }
        for (auto& child : box.children) search(child);
    };
    search(root);
    
    assert(found && "Should find div with 200px width");
    std::cout << "test_layout_block_width: PASSED\n";
}

void test_render_tree_build() {
    auto doc = parseHTML("<html><body><div style=\"background-color: red;\">Red box</div></body></html>");
    LayoutEngine engine(800, 600);
    auto layoutRoot = engine.layout(*doc);
    
    auto renderTree = buildRenderTree(layoutRoot);
    assert(renderTree != nullptr && "Should build render tree");
    std::cout << "test_render_tree_build: PASSED\n";
}

void test_render_commands() {
    auto doc = parseHTML("<html><body><div style=\"background-color: blue; width: 100px; height: 100px;\"></div></body></html>");
    LayoutEngine engine(800, 600);
    auto layoutRoot = engine.layout(*doc);
    
    auto renderTree = buildRenderTree(layoutRoot);
    assert(renderTree != nullptr);
    
    RenderTreeFlattener flattener;
    auto commands = flattener.flatten(*renderTree);
    
    bool foundBlueRect = false;
    for (auto& cmd : commands) {
        if (cmd.type == CommandType::DRAW_RECT) {
            if (cmd.fillColor.b > 0.9f) {
                foundBlueRect = true;
            }
        }
    }
    assert(foundBlueRect && "Should find blue rectangle draw command");
    std::cout << "test_render_commands: PASSED\n";
}

void test_gpu_renderer() {
    GPURenderer renderer(100, 100);
    renderer.beginFrame();
    
    DrawCommand cmd;
    cmd.type = CommandType::DRAW_RECT;
    cmd.bounds = Rect(10, 10, 80, 80);
    cmd.fillColor = Color(1, 0, 0, 1); // Red
    
    CommandList commands = {cmd};
    renderer.executeCommands(commands);
    renderer.endFrame();
    
    const auto& fb = renderer.framebuffer();
    assert(!fb.empty() && "Framebuffer should not be empty");
    
    uint32_t pixel = fb[50 * 100 + 50];
    uint8_t r = (pixel >> 16) & 0xFF;
    uint8_t g = (pixel >> 8) & 0xFF;
    uint8_t b = pixel & 0xFF;
    assert(r > 200 && g < 50 && b < 50 && "Pixel should be red");
    
    std::cout << "test_gpu_renderer: PASSED\n";
}

void test_full_pipeline() {
    std::string html = R"(
<!DOCTYPE html>
<html>
<head><title>Pipeline Test</title></head>
<body>
    <div style="background-color: green; width: 400px; height: 300px;">
        <p style="color: white;">Green box</p>
    </div>
</body>
</html>
)";
    
    HtmlLexer lexer(html);
    auto tokens = lexer.tokenize();
    assert(!tokens.empty() && "Should tokenize");
    
    HtmlParser parser(tokens);
    auto doc = parser.parse();
    assert(doc != nullptr && "Should parse");
    assert(doc->title == "Pipeline Test" && "Should extract title");
    
    LayoutEngine layoutEngine(800, 600);
    auto layoutRoot = layoutEngine.layout(*doc);
    
    auto renderTree = buildRenderTree(layoutRoot);
    assert(renderTree != nullptr && "Should build render tree");
    
    RenderTreeFlattener flattener;
    auto commands = flattener.flatten(*renderTree);
    
    GPURenderer gpuRenderer(800, 600);
    gpuRenderer.beginFrame();
    gpuRenderer.executeCommands(commands);
    gpuRenderer.endFrame();
    
    std::cout << "test_full_pipeline: PASSED (commands=" << commands.size() << ")\n";
}

int main() {
    std::cout << "=== Layout Tests ===" << std::endl;
    test_layout_viewport();
    test_layout_block_width();
    test_render_tree_build();
    test_render_commands();
    test_gpu_renderer();
    test_full_pipeline();
    std::cout << "All layout tests passed!" << std::endl;
    return 0;
}
