#include "src/dom/style.h"
#include "include/utils/string_utils.h"
#include <stdexcept>
#include <variant>

namespace VEOEngine {

static StyleValue parseLength(const std::string& val) {
    std::string v = trim(val);
    if (v == "auto") return StyleValueAuto{};
    if (!v.empty() && v.back() == '%') {
        try { return StyleValuePercent{std::stof(v.substr(0, v.size()-1))}; }
        catch (...) {}
    }
    if (v.size() >= 2 && v.substr(v.size()-2) == "px") {
        try { return StyleValuePx{std::stof(v.substr(0, v.size()-2))}; }
        catch (...) {}
    }
    try { return StyleValuePx{std::stof(v)}; }
    catch (...) {}
    return StyleValueAuto{};
}

static std::vector<StyleValue> parseEdgeValues(const std::string& val) {
    auto parts = split(trim(val), ' ');
    std::vector<std::string> filtered;
    for (auto& p : parts) {
        std::string t = trim(p);
        if (!t.empty()) filtered.push_back(t);
    }
    std::vector<StyleValue> result;
    for (auto& p : filtered) result.push_back(parseLength(p));
    return result;
}

Style Style::parse(const std::string& inlineStyle) {
    Style s;
    auto decls = split(inlineStyle, ';');
    for (auto& decl : decls) {
        auto trimmed = trim(decl);
        if (trimmed.empty()) continue;
        auto colon = trimmed.find(':');
        if (colon == std::string::npos) continue;
        std::string prop = trim(toLower(trimmed.substr(0, colon)));
        std::string val = trim(trimmed.substr(colon + 1));
        s.applyProperty(prop, val);
    }
    return s;
}

void Style::applyProperty(const std::string& property, const std::string& value) {
    std::string v = trim(value);
    std::string lv = toLower(v);
    
    if (property == "display") {
        if (lv == "block") display = Display::Block;
        else if (lv == "inline") display = Display::Inline;
        else if (lv == "inline-block") display = Display::InlineBlock;
        else if (lv == "none") display = Display::None;
        else if (lv == "flex") display = Display::Flex;
    } else if (property == "position") {
        if (lv == "static") position = Position::Static;
        else if (lv == "relative") position = Position::Relative;
        else if (lv == "absolute") position = Position::Absolute;
        else if (lv == "fixed") position = Position::Fixed;
    } else if (property == "float") {
        if (lv == "left") floatProp = FloatProp::Left;
        else if (lv == "right") floatProp = FloatProp::Right;
        else floatProp = FloatProp::None;
    } else if (property == "width") {
        width = parseLength(v);
    } else if (property == "height") {
        height = parseLength(v);
    } else if (property == "margin") {
        auto vals = parseEdgeValues(v);
        if (vals.size() == 1) {
            margin.top = margin.right = margin.bottom = margin.left = vals[0];
        } else if (vals.size() == 2) {
            margin.top = margin.bottom = vals[0];
            margin.right = margin.left = vals[1];
        } else if (vals.size() == 3) {
            margin.top = vals[0];
            margin.right = margin.left = vals[1];
            margin.bottom = vals[2];
        } else if (vals.size() >= 4) {
            margin.top = vals[0];
            margin.right = vals[1];
            margin.bottom = vals[2];
            margin.left = vals[3];
        }
    } else if (property == "margin-top")    { margin.top = parseLength(v); }
    else if (property == "margin-right")    { margin.right = parseLength(v); }
    else if (property == "margin-bottom")   { margin.bottom = parseLength(v); }
    else if (property == "margin-left")     { margin.left = parseLength(v); }
    else if (property == "padding") {
        auto vals = parseEdgeValues(v);
        if (vals.size() == 1) {
            padding.top = padding.right = padding.bottom = padding.left = vals[0];
        } else if (vals.size() == 2) {
            padding.top = padding.bottom = vals[0];
            padding.right = padding.left = vals[1];
        } else if (vals.size() == 3) {
            padding.top = vals[0];
            padding.right = padding.left = vals[1];
            padding.bottom = vals[2];
        } else if (vals.size() >= 4) {
            padding.top = vals[0];
            padding.right = vals[1];
            padding.bottom = vals[2];
            padding.left = vals[3];
        }
    } else if (property == "padding-top")    { padding.top = parseLength(v); }
    else if (property == "padding-right")    { padding.right = parseLength(v); }
    else if (property == "padding-bottom")   { padding.bottom = parseLength(v); }
    else if (property == "padding-left")     { padding.left = parseLength(v); }
    else if (property == "background-color" || property == "background") {
        backgroundColor = parseColor(v);
    } else if (property == "color") {
        color = parseColor(v);
    } else if (property == "font-size") {
        if (endsWith(v, "px")) {
            try { fontSize = std::stof(v.substr(0, v.size()-2)); } catch (...) {}
        } else {
            try { fontSize = std::stof(v); } catch (...) {}
        }
    } else if (property == "font-weight") {
        if (lv == "bold") fontWeight = 700;
        else if (lv == "normal") fontWeight = 400;
        else { try { fontWeight = std::stoi(v); } catch (...) {} }
    } else if (property == "font-family") {
        fontFamily = v;
    } else if (property == "text-align") {
        if (lv == "left") textAlign = TextAlign::Left;
        else if (lv == "center") textAlign = TextAlign::Center;
        else if (lv == "right") textAlign = TextAlign::Right;
        else if (lv == "justify") textAlign = TextAlign::Justify;
    } else if (property == "overflow") {
        if (lv == "hidden") overflow = Overflow::Hidden;
        else if (lv == "scroll") overflow = Overflow::Scroll;
        else if (lv == "auto") overflow = Overflow::Auto;
        else overflow = Overflow::Visible;
    } else if (property == "z-index") {
        try { zIndex = std::stof(v); } catch (...) {}
    } else if (property == "opacity") {
        try { opacity = std::stof(v); } catch (...) {}
    } else if (property == "flex-direction") {
        if (lv == "row") flexDirection = FlexDirection::Row;
        else if (lv == "column") flexDirection = FlexDirection::Column;
        else if (lv == "row-reverse") flexDirection = FlexDirection::RowReverse;
        else if (lv == "column-reverse") flexDirection = FlexDirection::ColumnReverse;
    } else if (property == "border-width") {
        if (endsWith(v, "px")) {
            try { border.width = std::stof(v.substr(0, v.size()-2)); } catch (...) {}
        } else {
            try { border.width = std::stof(v); } catch (...) {}
        }
    } else if (property == "border-color") {
        border.color = parseColor(v);
    } else if (property == "border-style") {
        if (lv == "solid") border.style = BorderStyle::Solid;
        else if (lv == "dashed") border.style = BorderStyle::Dashed;
        else if (lv == "dotted") border.style = BorderStyle::Dotted;
        else if (lv == "double") border.style = BorderStyle::Double;
        else border.style = BorderStyle::None;
    } else if (property == "border") {
        auto parts = split(v, ' ');
        for (auto& p : parts) {
            std::string tp = trim(p);
            std::string ltp = toLower(tp);
            if (endsWith(tp, "px")) {
                try { border.width = std::stof(tp.substr(0, tp.size()-2)); } catch (...) {}
            } else if (ltp == "solid") border.style = BorderStyle::Solid;
            else if (ltp == "dashed") border.style = BorderStyle::Dashed;
            else if (ltp == "dotted") border.style = BorderStyle::Dotted;
            else if (!tp.empty()) border.color = parseColor(tp);
        }
    }
}

float Style::resolveWidth(float containerWidth) const {
    return std::visit([&](auto&& val) -> float {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, StyleValuePx>) return val.value;
        if constexpr (std::is_same_v<T, StyleValuePercent>) return containerWidth * val.value / 100.0f;
        return containerWidth;
    }, width);
}

float Style::resolveHeight(float containerHeight) const {
    return std::visit([&](auto&& val) -> float {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, StyleValuePx>) return val.value;
        if constexpr (std::is_same_v<T, StyleValuePercent>) return containerHeight * val.value / 100.0f;
        return 0.0f;
    }, height);
}

} // namespace VEOEngine
