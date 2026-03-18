#ifndef EXECUTE_H
#define EXECUTE_H
#include <string>
#include <memory>

namespace VEOEngine {

struct RendererConfig {
    int width = 800;
    int height = 600;
    bool enableVEO = true;
    bool softwareFallback = true;
    std::string fontPath;
};

class RendererImpl;

class Renderer {
public:
    Renderer();
    explicit Renderer(const RendererConfig& config);
    ~Renderer();

    void loadHTML(const std::string& html);
    void render();
    void update();
    void shutdown();
    void resize(int width, int height);
    void setDebugMode(bool enabled);

private:
    std::unique_ptr<RendererImpl> impl_;
};

} // namespace VEOEngine
#endif
