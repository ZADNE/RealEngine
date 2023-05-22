/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/window/WindowSettings.hpp>

#include <fstream>
#include <filesystem>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

const std::string SETTINGS_FILENAME = "settings.json";

namespace RE {

WindowSettings::WindowSettings() {
    std::ifstream i(SETTINGS_FILENAME);
    json j;
    try {
        i >> j;
        m_dims.x = j["window"]["width"].get<int>();
        m_dims.y = j["window"]["height"].get<int>();
        m_flags.fullscreen = j["window"]["fullscreen"].get<bool>();
        m_flags.borderless = j["window"]["borderless"].get<bool>();
        m_flags.vSync = j["window"]["vsync"].get<bool>();
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
    }
    catch (...) {
        //Settings either don't exist or are currupted
        i.close();
        if (std::filesystem::exists(SETTINGS_FILENAME)) {
            //Back up previous settings
            std::filesystem::rename(SETTINGS_FILENAME, SETTINGS_FILENAME + ".backup");
        }
        reset();//Reset settings to default values
        save();
    }
}

WindowSettings::WindowSettings(const glm::ivec2& dims, WindowFlags flags, RendererID preferredRenderer) :
    m_dims(dims), m_flags(flags), m_preferredRenderer(preferredRenderer) {

}

void WindowSettings::reset() {
    m_dims = glm::vec2(1280.0f, 1000.0f);
    m_flags = WindowFlags{};
    m_preferredRenderer = RendererID::Any;
}

void WindowSettings::save() {
    nlohmann::ordered_json j = {
        {"window", {
            {"width", (unsigned int)m_dims.x},
            {"height", (unsigned int)m_dims.y},
            {"fullscreen", (bool)m_flags.fullscreen},
            {"borderless", (bool)m_flags.borderless},
            {"vsync", (bool)m_flags.vSync},
            {"preferred_renderer", to_string(m_preferredRenderer)}
        }}
    };

    std::ofstream o(SETTINGS_FILENAME, std::ofstream::trunc);
    o << j.dump(2);
    o.close();
}

}