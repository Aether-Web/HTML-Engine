#pragma once
#include "src/render/render_command.h"
#include "include/std/types.h"

namespace VEOEngine {

class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void executeCommands(const CommandList& commands) = 0;
    virtual void clear(const Color& color) = 0;
    virtual void resize(int width, int height) = 0;
};

} // namespace VEOEngine
