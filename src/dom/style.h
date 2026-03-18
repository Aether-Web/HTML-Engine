#pragma once
#include <string>
#include <variant>
#include "include/std/types.h"

namespace VEOEngine {

enum class Display { Block, Inline, InlineBlock, None, Flex };
enum class Position { Static, Relative, Absolute, Fixed };
enum class FloatProp { None, Left, Right };
enum class TextAlign { Left, Center, Right, Justify };
enum class Overflow { Visible, Hidden, Scroll, Auto };
enum class FlexDirection { Row, Column, RowReverse, ColumnReverse };
enum class JustifyContent { FlexStart, FlexEnd, Center, SpaceBetween, SpaceAround };
enum class AlignItems { FlexStart, FlexEnd, Center, Stretch, Baseline };
enum class BorderStyle { None, Solid, Dashed, Dotted, Double };

struct StyleValueAuto {};
struct StyleValuePx { float value; };
struct StyleValuePercent { float value; };
struct StyleValueKeyword { std::string value; };

using StyleValue = std::variant<StyleValueAuto, StyleValuePx, StyleValuePercent, StyleValueKeyword>;

struct StyleEdgeSizes {
    StyleValue top{StyleValuePx{0}};
    StyleValue right{StyleValuePx{0}};
    StyleValue bottom{StyleValuePx{0}};
    StyleValue left{StyleValuePx{0}};
};

struct BorderProperties {
    float width = 0;
    BorderStyle style = BorderStyle::None;
    Color color{0,0,0,1};
};

struct Style {
    Display display = Display::Block;
    Position position = Position::Static;
    FloatProp floatProp = FloatProp::None;
    
    StyleValue width{StyleValueAuto{}};
    StyleValue height{StyleValueAuto{}};
    
    StyleEdgeSizes margin;
    StyleEdgeSizes padding;
    BorderProperties border;
    
    Color backgroundColor{0,0,0,0};
    Color color{0,0,0,1};
    
    float fontSize = 16.0f;
    int fontWeight = 400;
    std::string fontFamily = "sans-serif";
    
    TextAlign textAlign = TextAlign::Left;
    Overflow overflow = Overflow::Visible;
    
    FlexDirection flexDirection = FlexDirection::Row;
    JustifyContent justifyContent = JustifyContent::FlexStart;
    AlignItems alignItems = AlignItems::Stretch;
    
    float zIndex = 0;
    float opacity = 1.0f;
    
    static Style parse(const std::string& inlineStyle);
    void applyProperty(const std::string& property, const std::string& value);
    
    float resolveWidth(float containerWidth) const;
    float resolveHeight(float containerHeight) const;
};

} // namespace VEOEngine
