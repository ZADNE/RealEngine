﻿/**
 *  @author    Dubsky Tomas
 */
#include <filesystem>
#include <fstream>

#include <RealEngine/window/WindowSettings.hpp>

#include <nlohmann/json.hpp>

namespace {

const std::string k_settingFilename = "settings.json";
constexpr glm::vec2 k_defaultWindowSize{1280.0f, 1000.0f};

} // namespace

namespace re {

WindowSettings::WindowSettings() {
    std::ifstream i(k_settingFilename);
    nlohmann::json j;
    try {
        i >> j;
        m_dims.x           = j["window"]["width"].get<int>();
        m_dims.y           = j["window"]["height"].get<int>();
        m_flags.fullscreen = j["window"]["fullscreen"].get<bool>();
        m_flags.borderless = j["window"]["borderless"].get<bool>();
        m_flags.vSync      = j["window"]["vsync"].get<bool>();
        m_preferredDevice = j["window"]["preferred_device"].get<std::string_view>();
        auto renderer = j["window"].find("preferred_renderer");
        if (renderer != j["window"].end()) {
            auto renderStr = renderer->get<std::string>();
            if (renderStr == to_string(RendererID::Vulkan13)) {
                m_preferredRenderer = RendererID::Vulkan13;
            } else if (renderStr == to_string(RendererID::Any)) {
                m_preferredRenderer = RendererID::Any;
            } else {
                m_preferredRenderer = RendererID::Any;
                save();
            }
        } else {
            m_preferredRenderer = RendererID::Any;
            save();
        }
    } catch (...) {
        // Settings either don't exist or are currupted
        i.close();
        if (std::filesystem::exists(k_settingFilename)) {
            // Back up previous settings
            std::filesystem::rename(
                k_settingFilename, k_settingFilename + ".backup"
            );
        }
        reset(); // Reset settings to default values
        save();
    }
}

WindowSettings::WindowSettings(
    glm::ivec2 dims, WindowFlags flags, RendererID preferredRenderer,
    std::string_view preferredDevice
)
    : m_dims(dims)
    , m_flags(flags)
    , m_preferredRenderer(preferredRenderer)
    , m_preferredDevice(preferredDevice) {
}

void WindowSettings::reset() {
    m_dims              = k_defaultWindowSize;
    m_flags             = WindowFlags{};
    m_preferredRenderer = RendererID::Any;
    m_preferredDevice.clear();
}

void WindowSettings::save() {
    nlohmann::ordered_json j{
        {"window",
         {{"width", (unsigned int)m_dims.x},
          {"height", (unsigned int)m_dims.y},
          {"fullscreen", (bool)m_flags.fullscreen},
          {"borderless", (bool)m_flags.borderless},
          {"vsync", (bool)m_flags.vSync},
          {"preferred_device", m_preferredDevice},
          {"preferred_renderer", to_string(m_preferredRenderer)}}}
    };

    std::ofstream o(k_settingFilename, std::ofstream::trunc);
    o << j.dump(2);
    o.close();
}

} // namespace re
