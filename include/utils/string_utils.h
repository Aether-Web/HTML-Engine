#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include "include/std/types.h"

namespace VEOEngine {

inline std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\n\r\f\v");
    return s.substr(start, end - start + 1);
}

inline std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(s);
    while (std::getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

inline std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

inline std::string toUpper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return s;
}

inline bool startsWith(const std::string& s, const std::string& prefix) {
    return s.size() >= prefix.size() && s.substr(0, prefix.size()) == prefix;
}

inline bool endsWith(const std::string& s, const std::string& suffix) {
    return s.size() >= suffix.size() && s.substr(s.size() - suffix.size()) == suffix;
}

inline std::string join(const std::vector<std::string>& parts, const std::string& sep) {
    std::string result;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) result += sep;
        result += parts[i];
    }
    return result;
}

inline std::string replace(std::string s, const std::string& from, const std::string& to) {
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos) {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
    return s;
}

inline Color parseColor(const std::string& css) {
    std::string c = trim(css);
    if (c.empty()) return Color::Black();
    
    std::string lc = toLower(c);
    if (lc == "red")         return Color(1,0,0,1);
    if (lc == "green")       return Color(0,0.5f,0,1);
    if (lc == "blue")        return Color(0,0,1,1);
    if (lc == "white")       return Color(1,1,1,1);
    if (lc == "black")       return Color(0,0,0,1);
    if (lc == "transparent") return Color(0,0,0,0);
    if (lc == "yellow")      return Color(1,1,0,1);
    if (lc == "orange")      return Color(1,0.647f,0,1);
    if (lc == "purple")      return Color(0.5f,0,0.5f,1);
    if (lc == "gray" || lc == "grey") return Color(0.5f,0.5f,0.5f,1);
    if (lc == "cyan")        return Color(0,1,1,1);
    if (lc == "magenta")     return Color(1,0,1,1);
    if (lc == "pink")        return Color(1,0.753f,0.796f,1);
    if (lc == "brown")       return Color(0.647f,0.165f,0.165f,1);
    if (lc == "navy")        return Color(0,0,0.5f,1);
    if (lc == "teal")        return Color(0,0.5f,0.5f,1);
    if (lc == "silver")      return Color(0.753f,0.753f,0.753f,1);
    
    if (c[0] == '#') {
        std::string hex = c.substr(1);
        if (hex.size() == 3) {
            hex = std::string(2, hex[0]) + std::string(2, hex[1]) + std::string(2, hex[2]);
        }
        if (hex.size() >= 6) {
            try {
                uint32_t val = std::stoul(hex.substr(0,6), nullptr, 16);
                float r = ((val >> 16) & 0xFF) / 255.0f;
                float g = ((val >> 8) & 0xFF) / 255.0f;
                float b = (val & 0xFF) / 255.0f;
                float a = 1.0f;
                if (hex.size() >= 8) {
                    uint32_t av = std::stoul(hex.substr(6,2), nullptr, 16);
                    a = av / 255.0f;
                }
                return Color(r,g,b,a);
            } catch (...) {}
        }
    }
    
    if (startsWith(lc, "rgb")) {
        size_t start = c.find('(');
        size_t end = c.find(')');
        if (start != std::string::npos && end != std::string::npos) {
            auto parts = split(c.substr(start+1, end-start-1), ',');
            if (parts.size() >= 3) {
                try {
                    float r = std::stof(trim(parts[0])) / 255.0f;
                    float g = std::stof(trim(parts[1])) / 255.0f;
                    float b = std::stof(trim(parts[2])) / 255.0f;
                    float a = 1.0f;
                    if (parts.size() >= 4) a = std::stof(trim(parts[3]));
                    return Color(r,g,b,a);
                } catch (...) {}
            }
        }
    }
    
    return Color::Black();
}

} // namespace VEOEngine
