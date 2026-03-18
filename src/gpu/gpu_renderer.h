#pragma once
#include "src/render/renderer.h"
#include <vector>
#include <string>
#include <cstdint>

namespace VEOEngine {

class GPURenderer : public IRenderer {
public:
    GPURenderer(int width, int height);
    ~GPURenderer() override = default;
    
    void beginFrame() override;
    void endFrame() override;
    void executeCommands(const CommandList& commands) override;
    void clear(const Color& color) override;
    void resize(int width, int height) override;
    
    bool saveFrame(const std::string& filename) const;
    const std::vector<uint32_t>& framebuffer() const { return framebuffer_; }
    int width() const { return width_; }
    int height() const { return height_; }

private:
    int width_, height_;
    std::vector<uint32_t> framebuffer_;
    
    void drawRect(const Rect& bounds, const Color& color);
    void drawRoundedRect(const Rect& bounds, const Color& color, float radius);
    void drawBorder(const Rect& bounds, const Color& color, float bwidth);
    void drawText(const std::string& text, const Rect& bounds, const Color& color, float fontSize);
    void setPixel(int x, int y, const Color& color);
    void blendPixel(int x, int y, const Color& color);
    uint32_t colorToUint32(const Color& c) const;
    Color uint32ToColor(uint32_t c) const;
};

} // namespace VEOEngine
