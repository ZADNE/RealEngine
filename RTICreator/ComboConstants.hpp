/*! 
 *  @author    Dubsky Tomas
 */
#pragma once

#include <string>
#include <array>

#include <ImGui/imgui.h>
#include <RealEngine/rendering/textures/Texture.hpp>

static constexpr std::array MIN_FILTERS = {
    RE::TextureMinFilter::NEAREST_NO_MIPMAPS, RE::TextureMinFilter::LINEAR_NO_MIPMAPS,
    RE::TextureMinFilter::NEAREST_MIPMAP_NEAREST, RE::TextureMinFilter::NEAREST_MIPMAP_LINEAR,
    RE::TextureMinFilter::LINEAR_MIPMAP_NEAREST, RE::TextureMinFilter::LINEAR_MIPMAP_LINEAR
};

static constexpr std::array MIN_FILTERS_LABELS = {
    "Nearest - no mipmaps", "Linear - no mipmaps",
    "Nearest in mipmap, nearest between mipmaps", "Nearest in mipmap, linear between mipmaps",
    "Linear in mipmap, nearest between mipmaps", "Linear in mipmap, linear between mipmaps"
};

static constexpr std::array MAG_FILTERS = {
    RE::TextureMagFilter::NEAREST, RE::TextureMagFilter::LINEAR,
};

static constexpr std::array MAG_FILTERS_LABELS = {
    "Nearest", "Linear"
};

static constexpr std::array WRAP_STYLES = {
    RE::TextureWrapStyle::CLAMP_TO_EDGE, RE::TextureWrapStyle::CLAMP_TO_BORDER,
    RE::TextureWrapStyle::REPEAT_NORMALLY, RE::TextureWrapStyle::REPEAT_MIRRORED
};

static constexpr std::array WRAP_STYLES_LABELS = {
    "Clamp to edge", "Clamp to border color",
    "Repeat normally", "Repeat mirrored"
};

template<size_t size>
bool imguiCombo(const char* label, size_t& selector, const std::array<const char*, size>& labels) {
    bool rval = false;
    if (ImGui::BeginCombo(label, labels[selector])) {
        for (size_t i = 0; i < labels.size(); ++i) {
            if (ImGui::Selectable(labels[i], i == selector)) {
                selector = i;
                rval = true;
            }
        }
        ImGui::EndCombo();
    }
    return rval;
}

template<typename T, size_t size>
size_t findSelectedFromCombo(T value, const std::array<T, size>& values) {
    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i] == value) {
            return i;
        }
    }
    return 0;
}
