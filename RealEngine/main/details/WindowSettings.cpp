#include <RealEngine/main/details/WindowSettings.hpp>

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
		p_dims.x = j["window"]["width"].get<int>();
		p_dims.y = j["window"]["height"].get<int>();
		p_flags.fullscreen = j["window"]["fullscreen"].get<bool>();
		p_flags.borderless = j["window"]["borderless"].get<bool>();
		p_flags.vSync = j["window"]["vsync"].get<bool>();
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

WindowSettings::WindowSettings(const glm::ivec2& dims, WindowFlags flags) :
	p_dims(dims), p_flags(flags) {

}

void WindowSettings::reset() {
	p_dims = glm::vec2(1280.0f, 1000.0f);
	p_flags = WindowFlags{};
}

void WindowSettings::save() {
	nlohmann::ordered_json j = {
		{"window", {
			{"width", (unsigned int)p_dims.x},
			{"height", (unsigned int)p_dims.y},
			{"fullscreen", (bool)p_flags.fullscreen},
			{"borderless", (bool)p_flags.borderless},
			{"vsync", (bool)p_flags.vSync}
		}}
	};

	std::ofstream o(SETTINGS_FILENAME, std::ofstream::trunc);
	o << j.dump(2);
	o.close();
}

}