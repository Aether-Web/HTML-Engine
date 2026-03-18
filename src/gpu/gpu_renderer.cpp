#include "src/gpu/gpu_renderer.h"
#include <algorithm>
#include <fstream>
#include <cmath>

namespace VEOEngine {

GPURenderer::GPURenderer(int width, int height)
    : width_(width), height_(height), framebuffer_(width * height, 0xFFFFFFFF) {}

void GPURenderer::resize(int width, int height) {
    width_ = width;
    height_ = height;
    framebuffer_.assign(width * height, 0xFFFFFFFF);
}

void GPURenderer::beginFrame() {
    clear(Color::White());
}

void GPURenderer::endFrame() {}

uint32_t GPURenderer::colorToUint32(const Color& c) const {
    uint8_t r = (uint8_t)(std::clamp(c.r, 0.0f, 1.0f) * 255);
    uint8_t g = (uint8_t)(std::clamp(c.g, 0.0f, 1.0f) * 255);
    uint8_t b = (uint8_t)(std::clamp(c.b, 0.0f, 1.0f) * 255);
    uint8_t a = (uint8_t)(std::clamp(c.a, 0.0f, 1.0f) * 255);
    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

Color GPURenderer::uint32ToColor(uint32_t c) const {
    float a = ((c >> 24) & 0xFF) / 255.0f;
    float r = ((c >> 16) & 0xFF) / 255.0f;
    float g = ((c >> 8) & 0xFF) / 255.0f;
    float b = (c & 0xFF) / 255.0f;
    return Color(r, g, b, a);
}

void GPURenderer::setPixel(int x, int y, const Color& color) {
    if (x < 0 || x >= width_ || y < 0 || y >= height_) return;
    framebuffer_[y * width_ + x] = colorToUint32(color);
}

void GPURenderer::blendPixel(int x, int y, const Color& color) {
    if (x < 0 || x >= width_ || y < 0 || y >= height_) return;
    if (color.a >= 1.0f) {
        setPixel(x, y, color);
        return;
    }
    Color dst = uint32ToColor(framebuffer_[y * width_ + x]);
    float alpha = color.a;
    Color blended(
        color.r * alpha + dst.r * (1 - alpha),
        color.g * alpha + dst.g * (1 - alpha),
        color.b * alpha + dst.b * (1 - alpha),
        1.0f
    );
    setPixel(x, y, blended);
}

void GPURenderer::clear(const Color& color) {
    uint32_t c = colorToUint32(color);
    std::fill(framebuffer_.begin(), framebuffer_.end(), c);
}

void GPURenderer::drawRect(const Rect& bounds, const Color& color) {
    int x0 = (int)bounds.x;
    int y0 = (int)bounds.y;
    int x1 = (int)(bounds.x + bounds.width);
    int y1 = (int)(bounds.y + bounds.height);
    for (int y = y0; y < y1; ++y) {
        for (int x = x0; x < x1; ++x) {
            blendPixel(x, y, color);
        }
    }
}

void GPURenderer::drawRoundedRect(const Rect& bounds, const Color& color, float /*radius*/) {
    drawRect(bounds, color);
}

void GPURenderer::drawBorder(const Rect& bounds, const Color& color, float bw) {
    if (bw <= 0) return;
    drawRect(Rect(bounds.x, bounds.y, bounds.width, bw), color);
    drawRect(Rect(bounds.x, bounds.y + bounds.height - bw, bounds.width, bw), color);
    drawRect(Rect(bounds.x, bounds.y, bw, bounds.height), color);
    drawRect(Rect(bounds.x + bounds.width - bw, bounds.y, bw, bounds.height), color);
}

void GPURenderer::drawText(const std::string& text, const Rect& bounds, const Color& color, float fontSize) {
    float charWidth = fontSize * 0.6f;
    float lineHeight = fontSize * 1.2f;
    float w = std::min((float)text.size() * charWidth, bounds.width);
    float h = std::min(lineHeight, bounds.height);
    drawRect(Rect(bounds.x, bounds.y, w, h), color);
}

void GPURenderer::executeCommands(const CommandList& commands) {
    for (const auto& cmd : commands) {
        switch (cmd.type) {
            case CommandType::CLEAR:
                clear(cmd.fillColor);
                break;
            case CommandType::DRAW_RECT:
                drawRect(cmd.bounds, cmd.fillColor);
                if (cmd.strokeWidth > 0) {
                    drawBorder(cmd.bounds, cmd.strokeColor, cmd.strokeWidth);
                }
                break;
            case CommandType::DRAW_ROUNDED_RECT:
                drawRoundedRect(cmd.bounds, cmd.fillColor, cmd.cornerRadius);
                if (cmd.strokeWidth > 0) {
                    drawBorder(cmd.bounds, cmd.strokeColor, cmd.strokeWidth);
                }
                break;
            case CommandType::DRAW_TEXT:
                drawText(cmd.text, cmd.bounds, cmd.fillColor, cmd.fontSize);
                break;
            case CommandType::DRAW_IMAGE:
                drawRect(cmd.bounds, Color(0.8f, 0.8f, 0.8f, 1.0f));
                break;
            default:
                break;
        }
    }
}

bool GPURenderer::saveFrame(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return false;
    file << "P6\n" << width_ << " " << height_ << "\n255\n";
    for (uint32_t pixel : framebuffer_) {
        uint8_t r = (pixel >> 16) & 0xFF;
        uint8_t g = (pixel >> 8) & 0xFF;
        uint8_t b = pixel & 0xFF;
        file.write(reinterpret_cast<const char*>(&r), 1);
        file.write(reinterpret_cast<const char*>(&g), 1);
        file.write(reinterpret_cast<const char*>(&b), 1);
    }
    return true;
}

} // namespace VEOEngine
