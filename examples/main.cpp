#include "include/execute.h"
#include "src/lexer/html_lexer.h"
#include "src/parser/html_parser.h"
#include "src/layout/layout_engine.h"
#include "src/render/render_tree.h"
#include "src/gpu/gpu_renderer.h"
#include "src/veo/veo_runtime.h"
#include <iostream>
#include <string>

int main() {
    std::string html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>HTML Engine Test</title>
</head>
<body>
    <div id="header" style="background-color: navy; color: white; padding: 10px;">
        <h1>Hello, HTML Engine!</h1>
    </div>
    <div id="content" style="padding: 20px;">
        <p style="color: black;">This is a paragraph.</p>
        <div style="background-color: red; width: 100px; height: 50px;"></div>
        <ul>
            <li>Item 1</li>
            <li>Item 2</li>
            <li>Item 3</li>
        </ul>
    </div>
    <div id="footer" style="background-color: gray; padding: 5px;">
        <p>Footer</p>
    </div>
    <script type="veo">
log(Page loaded successfully)
    </script>
</body>
</html>
)";

    std::cout << "=== HTML Engine Demo ===" << std::endl;

    VEOEngine::RendererConfig config;
    config.width = 800;
    config.height = 600;
    config.enableVEO = true;
    config.softwareFallback = true;

    VEOEngine::Renderer renderer(config);
    renderer.setDebugMode(true);
    renderer.loadHTML(html);
    renderer.render();

    std::cout << "Render complete." << std::endl;

    std::cout << "\n=== Pipeline Demo ===" << std::endl;
    
    VEOEngine::HtmlLexer lexer(html);
    auto tokens = lexer.tokenize();
    std::cout << "Tokens: " << tokens.size() << std::endl;
    
    VEOEngine::HtmlParser parser(tokens);
    auto doc = parser.parse();
    std::cout << "Title: " << doc->title << std::endl;
    
    if (doc->body) {
        std::cout << "Body children: " << doc->body->children.size() << std::endl;
    }
    
    VEOEngine::LayoutEngine layoutEngine(800, 600);
    auto layoutRoot = layoutEngine.layout(*doc);
    std::cout << "Layout root children: " << layoutRoot.children.size() << std::endl;
    
    auto renderTree = VEOEngine::buildRenderTree(layoutRoot);
    std::cout << "Render tree built: " << (renderTree ? "yes" : "no") << std::endl;
    
    VEOEngine::RenderTreeFlattener flattener;
    if (renderTree) {
        auto commands = flattener.flatten(*renderTree);
        std::cout << "Draw commands: " << commands.size() << std::endl;
    }

    VEOEngine::GPURenderer gpuRenderer(800, 600);
    gpuRenderer.beginFrame();
    if (renderTree) {
        VEOEngine::RenderTreeFlattener f2;
        auto cmds = f2.flatten(*renderTree);
        gpuRenderer.executeCommands(cmds);
    }
    gpuRenderer.endFrame();
    
    if (gpuRenderer.saveFrame("output.ppm")) {
        std::cout << "Frame saved to output.ppm" << std::endl;
    }

    return 0;
}
