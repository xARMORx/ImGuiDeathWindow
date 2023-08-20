#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>

class Config
{
public:
	struct ConfigData
	{
		int BORDERSTYLE = 0;
		int FONTSIZE = 15;
		std::string FONTNAME = "Trebucbd";
	};

	static void Save();
	static void Load();

	static struct ConfigData* Get();
private:
	static std::filesystem::path GetConfig();
};