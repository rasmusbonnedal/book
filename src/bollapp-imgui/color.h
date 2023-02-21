#pragma once

#include <imgui.h>

#include <cmath>

inline float srgbToLinearFloat(float srgb) {
    if (srgb <= 0.04045f) {
        return srgb / 12.92f;
    } else {
        return std::powf((srgb + 0.055f) / 1.055f, 2.4f);
    }
}

inline float linearToSrgbFloat(float linear) {
    if (linear < 0.0031308f) {
        return linear * 12.92f;
    } else {
        return std::powf(linear, 1.0f / 2.4f) * 1.055f - 0.055f;
    }
}

class SrgbTable {
   public:
    SrgbTable();
    unsigned char convert(unsigned char v) const {
        return m_data[v];
    }

   private:
    unsigned char m_data[256];
};

extern const SrgbTable srgbTable;

inline ImVec4 srgbToLinear(const ImVec4& v) {
    return ImVec4(srgbToLinearFloat(v.x), srgbToLinearFloat(v.y), srgbToLinearFloat(v.z), v.w);
}

inline unsigned char srgbToLinearChar(unsigned char c) {
    return srgbTable.convert(c);
}

inline unsigned int srgbToLinearColor(unsigned int color) {
    unsigned char a = (color & 0xff000000) >> 24;
    unsigned char r = srgbToLinearChar((color & 0x00ff0000) >> 16);
    unsigned char g = srgbToLinearChar((color & 0x0000ff00) >> 8);
    unsigned char b = srgbToLinearChar((color & 0x000000ff) >> 0);
    return ((unsigned int)a << 24) | (r << 16) | (g << 8) | b;
}

void imguiAdjustSrgb();
