#include "Config.h"
#include <Windows.h>
#include <fstream>


struct Config::ConfigData ConfigData_;

struct Config::ConfigData* Config::Get() {
	return &ConfigData_;
}

using json = nlohmann::json;


void Config::Save() {

	json j;

	j["Settings"]["BORDERSTYLE"] = ConfigData_.BORDERSTYLE;
	j["Settings"]["FONTSIZE"] = ConfigData_.FONTSIZE;
	j["Settings"]["FONTNAME"] = ConfigData_.FONTNAME;


	std::ofstream(GetConfig().string()) << j.dump(4);
}

void Config::Load() {
	std::ifstream config(GetConfig().string());
	if (!config.is_open()) {
		return Save();
	}

	try {
		std::string content = std::string((std::istreambuf_iterator<char>(config)), std::istreambuf_iterator<char>());
		auto json = nlohmann::json::parse(content);

		// Settings
		json["Settings"]["BORDERSTYLE"].get_to<int>(ConfigData_.BORDERSTYLE);
		json["Settings"]["FONTSIZE"].get_to<int>(ConfigData_.FONTSIZE);
		json["Settings"]["FONTNAME"].get_to<std::string>(ConfigData_.FONTNAME);

	}
	catch (...) {
		Save();
	}

	config.close();
}

std::filesystem::path Config::GetConfig() {
	std::filesystem::path path = std::filesystem::current_path();
	std::filesystem::path name = "ImGuiDeathWindow";
	return (path / (name.string() + ".json"));
}