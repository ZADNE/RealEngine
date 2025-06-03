/**
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
        m_pos.x            = j["window"]["x"].get<int>();
        m_pos.y            = j["window"]["y"].get<int>();
        m_flags.fullscreen = j["window"]["fullscreen"].get<bool>();
        m_flags.borderless = j["window"]["borderless"].get<bool>();
        m_flags.vSync      = j["window"]["vsync"].get<bool>();
        m_preferredDevice = j["window"]["preferred_device"].get<std::string_view>();
    } catch (...) {
        // Settings either don't exist or are corrupted
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
    glm::ivec2 dims, glm::ivec2 pos, WindowFlags flags, std::string_view preferredDevice
)
    : m_dims{dims}
    , m_pos{pos}
    , m_flags{flags}
    , m_preferredDevice{preferredDevice} {
}

void WindowSettings::reset() {
    m_dims  = k_defaultWindowSize;
    m_pos   = glm::ivec2{k_centeredWindowPosition};
    m_flags = WindowFlags{};
    m_preferredDevice.clear();
}

void WindowSettings::save() {
    nlohmann::ordered_json j{
        {"window",
         {{"width", static_cast<int>(m_dims.x)},
          {"height", static_cast<int>(m_dims.y)},
          {"x", static_cast<int>(m_pos.x)},
          {"y", static_cast<int>(m_pos.y)},
          {"fullscreen", static_cast<bool>(m_flags.fullscreen)},
          {"borderless", static_cast<bool>(m_flags.borderless)},
          {"vsync", static_cast<bool>(m_flags.vSync)},
          {"preferred_device", m_preferredDevice}}}
    };

    std::ofstream o(k_settingFilename, std::ofstream::trunc);
    o << j.dump(2);
    o.close();
}

} // namespace re
