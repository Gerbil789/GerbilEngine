#include "EditorContext.h"
#include "Engine/Utility/Path.h"
#include "Editor/Utility/File.h"
#include "Engine/Core/Log.h"
#include <glaze/glaze.hpp>
#include <fstream>

namespace Editor
{
	EditorState EditorContext::state = EditorState::Edit;
	PlayMode EditorContext::playMode = PlayMode::PIE;
	Engine::Renderer EditorContext::renderer;
	Engine::Camera EditorContext::editorCamera;


	// =========================================================================
	// GLAZE DTO
	// =========================================================================

	struct EditorSettingsJSON
	{
		std::filesystem::path ProjectDirectory; // Glaze handles paths automatically!
	};
}

// =========================================================================
// GLAZE METADATA
// =========================================================================

template <>
struct glz::meta<Editor::EditorSettingsJSON> {
	using T = Editor::EditorSettingsJSON;
	static constexpr auto value = object(
		"ProjectDirectory", &T::ProjectDirectory
	);
};

// =========================================================================
// SETTINGS IMPLEMENTATION
// =========================================================================

namespace Editor
{
	void EditorSettings::Load()
	{
		std::filesystem::path settingsFilePath = GetSettingsFilePath("GerbilEditor");

		// Optional: If GetSettingsFilePath hardcodes a .yaml extension under the hood, 
		// you can force it to .json right here:
		settingsFilePath.replace_extension(".json");

		if (!std::filesystem::exists(settingsFilePath))
		{
			// If no settings file exists at all, prompt for directory and save
			projectDirectory = Editor::FileDialog::SelectDirectory();
			EditorSettings::Save();
			return;
		}

		EditorSettingsJSON data;
		std::string buffer;

		if (auto ec = glz::read_file_json(data, settingsFilePath.string(), buffer))
		{
			LOG_ERROR("Failed to parse Editor Settings: {}", glz::format_error(ec, buffer));
			return;
		}

		if (!data.ProjectDirectory.empty())
		{
			projectDirectory = data.ProjectDirectory;
		}
		else
		{
			projectDirectory = Editor::FileDialog::SelectDirectory();
			EditorSettings::Save();
		}

		LOG_INFO("Loaded settings from {}", settingsFilePath.string());
	}

	void EditorSettings::Save()
	{
		std::filesystem::path settingsFilePath = GetSettingsFilePath("GerbilEditor");
		settingsFilePath.replace_extension(".json"); // Ensure it writes as .json

		EditorSettingsJSON outData;
		outData.ProjectDirectory = projectDirectory;

		std::string buffer;
		if (auto ec = glz::write_file_json(outData, settingsFilePath.string(), buffer))
		{
			LOG_ERROR("Failed to save settings to {}: {}", settingsFilePath.string(), glz::format_error(ec));
			return;
		}

		LOG_INFO("Saved settings to {}", settingsFilePath.string());
	}
}