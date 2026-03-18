#include "include/execute.h"
#include "src/lexer/html_lexer.h"
#include "src/parser/html_parser.h"
#include "src/layout/layout_engine.h"
#include "src/render/render_tree.h"
#include "src/render/render_command.h"
#include "src/gpu/scene_graph.h"
#include "src/gpu/gpu_renderer.h"
#include "src/veo/veo_runtime.h"
#include <iostream>
#include <memory>

namespace VEOEngine {

class RendererImpl {
public:
    RendererConfig config;
    std::unique_ptr<Document> document;
    std::unique_ptr<GPURenderer> gpuRenderer;
    std::unique_ptr<LayoutEngine> layoutEngine;
    SceneGraph sceneGraph;
    bool debugMode = false;
    
    explicit RendererImpl(const RendererConfig& cfg) : config(cfg) {
        gpuRenderer = std::make_unique<GPURenderer>(cfg.width, cfg.height);
        layoutEngine = std::make_unique<LayoutEngine>(cfg.width, cfg.height);
    }
    
    void loadHTML(const std::string& html) {
        HtmlLexer lexer(html);
        auto tokens = lexer.tokenize();
        
        HtmlParser parser(tokens);
        document = parser.parse();
        
        if (config.enableVEO && document) {
            VEORuntime veoRuntime;
            VEODOMApi domApi(*document);
            auto scripts = veoRuntime.extractVEOScripts(*document);
            for (auto& script : scripts) {
                veoRuntime.execute(script, *document, domApi);
            }
        }
    }
    
    void render() {
        if (!document) return;
        
        auto layoutRoot = layoutEngine->layout(*document);
        auto renderTree = buildRenderTree(layoutRoot);
        
        if (!renderTree) return;
        
        RenderTreeFlattener flattener;
        auto commands = flattener.flatten(*renderTree);
        
        gpuRenderer->beginFrame();
        gpuRenderer->executeCommands(commands);
        gpuRenderer->endFrame();
        
        if (debugMode) {
            std::cout << "[Engine] Rendered " << commands.size() << " draw commands\n";
        }
    }
    
    void update() {
        render();
    }
    
    void shutdown() {
        document.reset();
    }
    
    void resize(int width, int height) {
        config.width = width;
        config.height = height;
        gpuRenderer->resize(width, height);
        layoutEngine = std::make_unique<LayoutEngine>(width, height);
    }
};

Renderer::Renderer() : impl_(std::make_unique<RendererImpl>(RendererConfig{})) {}

Renderer::Renderer(const RendererConfig& config) : impl_(std::make_unique<RendererImpl>(config)) {}

Renderer::~Renderer() = default;

void Renderer::loadHTML(const std::string& html) { impl_->loadHTML(html); }
void Renderer::render() { impl_->render(); }
void Renderer::update() { impl_->update(); }
void Renderer::shutdown() { impl_->shutdown(); }
void Renderer::resize(int width, int height) { impl_->resize(width, height); }
void Renderer::setDebugMode(bool enabled) { impl_->debugMode = enabled; }

} // namespace VEOEngine
