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
        auto renderer = j["window"]["renderer"].get<std::string>();
        if (renderer == to_string(RendererID::OPENGL_46)) {
            m_renderer = RendererID::OPENGL_46;
        } else {
            throw std::exception("Unknown renderer");
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

WindowSettings::WindowSettings(const glm::ivec2& dims, WindowFlags flags, RendererID renderer) :
    m_dims(dims), m_flags(flags), m_renderer(renderer) {

}

void WindowSettings::reset() {
    m_dims = glm::vec2(1280.0f, 1000.0f);
    m_flags = WindowFlags{};
    m_renderer = RendererID::OPENGL_46;
}

void WindowSettings::save() {
    nlohmann::ordered_json j = {
        {"window", {
            {"width", (unsigned int)m_dims.x},
            {"height", (unsigned int)m_dims.y},
            {"fullscreen", (bool)m_flags.fullscreen},
            {"borderless", (bool)m_flags.borderless},
            {"vsync", (bool)m_flags.vSync},
            {"renderer", to_string(m_renderer)}
        }}
    };

    std::ofstream o(SETTINGS_FILENAME, std::ofstream::trunc);
    o << j.dump(2);
    o.close();
}

}