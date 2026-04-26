#include "EditorContext.h"
#include "Engine/Utility/Path.h"
#include "Engine/Utility/Yaml.h"
#include "Engine/Utility/File.h"
#include "Engine/Core/Log.h"
#include <fstream>

namespace Editor
{
	void EditorSettings::Load()
	{
		std::filesystem::path settingsFilePath = GetSettingsFilePath("GerbilEditor");

		if (!std::filesystem::exists(settingsFilePath))
		{
			return;
		}

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(settingsFilePath.string());
		}
		catch (const YAML::ParserException& e)
		{
			LOG_ERROR("Failed to parse YAML: {}", e.what());
			return;
		}

		if (data["ProjectDirectory"])
		{
			projectDirectory = data["ProjectDirectory"].as<std::string>();
		}
		else
		{
			projectDirectory = Engine::OpenDirectory();
		}


		LOG_INFO("Loaded settings from {}", settingsFilePath);
		return;
	}

	void EditorSettings::Save()
	{
		std::filesystem::path settingsFilePath = GetSettingsFilePath("GerbilEditor");
		YAML::Emitter out;
		out << YAML::BeginMap;
		Engine::Yaml::Write(out, "ProjectDirectory", projectDirectory.string());
		out << YAML::EndMap;
		std::ofstream fout(settingsFilePath);
		if (!fout.is_open())
		{
			LOG_ERROR("Failed to open file for saving settings: {}", settingsFilePath);
			return;
		}
		fout << out.c_str();
		fout.close();
		LOG_INFO("Saved settings to {}", settingsFilePath);
	}
}