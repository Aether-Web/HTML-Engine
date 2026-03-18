#pragma once
#include <cstdint>
#include <string>

namespace VEOEngine {

struct Color {
    float r, g, b, a;
    Color(float r=0, float g=0, float b=0, float a=1) : r(r), g(g), b(b), a(a) {}
    static Color Black() { return {0,0,0,1}; }
    static Color White() { return {1,1,1,1}; }
    static Color Transparent() { return {0,0,0,0}; }
    static Color fromHex(uint32_t hex);
};

struct Vec2 {
    float x, y;
    Vec2(float x=0, float y=0) : x(x), y(y) {}
    Vec2 operator+(const Vec2& o) const { return {x+o.x, y+o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x-o.x, y-o.y}; }
};

struct Size {
    float width, height;
    Size(float w=0, float h=0) : width(w), height(h) {}
};

struct Rect {
    float x, y, width, height;
    Rect(float x=0, float y=0, float w=0, float h=0) : x(x), y(y), width(w), height(h) {}
    bool contains(const Vec2& p) const {
        return p.x >= x && p.x <= x+width && p.y >= y && p.y <= y+height;
    }
    Rect intersect(const Rect& o) const;
};

} // namespace VEOEngine
