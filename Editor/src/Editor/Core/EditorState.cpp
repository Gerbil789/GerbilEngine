#include "Editor/Core/EditorState.h"
#include "Editor/Utility/File.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Project.h"
#include <glaze/glaze.hpp>

struct EditorStateJSON
{
	std::filesystem::path projectPath;
};

std::filesystem::path Editor::GetProjectPath()
{
	const std::filesystem::path path = "editor_state.json";

	EditorStateJSON json;
	std::string buffer;

	// Load
	bool valid = true;

	if (!std::filesystem::exists(path))
	{
		LOG_WARNING("Editor state file does not exist: {}", path);
		valid = false;
	}
	else if (auto ec = glz::read_file_json(json, path.string(), buffer))
	{
		LOG_WARNING("Failed to parse Editor state: {}", glz::format_error(ec, buffer));
		valid = false;
	}
	else if (!std::filesystem::exists(json.projectPath))
	{
		LOG_WARNING("Project file does not exist: {}", json.projectPath);
		valid = false;
	}

	if (!valid)
	{
		json.projectPath = FileDialog::SelectFile({{ "Project Files", "project.json" }});
	}

	// Save
	if (auto ec = glz::write_file_json(json, path.string(), buffer))
	{
		LOG_ERROR("Failed to save state to {}: {}", path, glz::format_error(ec));;
	}

	return json.projectPath;
}