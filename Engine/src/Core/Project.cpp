#include "enginepch.h"
#include "Engine/Core/Project.h"
#include "Engine/Core/Log.h"
#include <glaze/glaze.hpp>

struct ProjectConfigJSON
{
	std::string title = "Untitled";
	uint64_t startScene = 0;
};

namespace Engine
{
	Project Project::s_ActiveProject;

	void Project::Load(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			LOG_ERROR("Config file not found at {}", path);
			return;
		}

		ProjectConfigJSON json;
		std::string buffer;

		if (auto ec = glz::read_file_json(json, path.string(), buffer))
		{
			LOG_ERROR("Failed to parse JSON in {}: {}", path, glz::format_error(ec, buffer));
			return;
		}

		Project project;
		project.m_ProjectDirectory = path.parent_path();
		project.m_AssetsDirectory = project.m_ProjectDirectory / "Assets";
		project.m_Title = json.title;
		project.m_DefaultSceneId = Engine::Uuid{json.startScene};

		LOG_INFO("Loaded project '{}' from {}", project.m_Title, path);
		s_ActiveProject = project;
	}

	Project& Project::GetActive()
	{
		return s_ActiveProject;
	}

	void Project::Save()
	{
		ProjectConfigJSON json;
		json.title = m_Title;
		json.startScene = static_cast<uint64_t>(m_DefaultSceneId);

		const std::filesystem::path path = m_ProjectDirectory / "project.json";
		std::string buffer;

		if (auto ec = glz::write_file_json(json, path.string(), buffer))
		{
			LOG_ERROR("Failed to save project to {}: {}", path, glz::format_error(ec));
			return;
		}

		LOG_INFO("Saved project to {}", path);
	}
}