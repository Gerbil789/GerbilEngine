#include "enginepch.h"
#include "Engine/Core/Project.h"
#include "Engine/Core/Log.h"
#include <glaze/glaze.hpp>
#include <fstream>

namespace Engine
{
	// =========================================================================
	// GLAZE DTO
	// =========================================================================

	struct ProjectConfigJSON
	{
		std::string Title = "Untitled";
		uint64_t StartScene = 0;
	};
}

// =========================================================================
// GLAZE METADATA
// =========================================================================

template <>
struct glz::meta<Engine::ProjectConfigJSON> {
	using T = Engine::ProjectConfigJSON;
	static constexpr auto value = object(
		"Title", &T::Title,
		"StartScene", &T::StartScene
	);
};

// =========================================================================
// PROJECT IMPLEMENTATION
// =========================================================================

namespace Engine
{
	Project Project::s_ActiveProject;

	Project& Project::New(const std::filesystem::path& path)
	{
		std::filesystem::create_directory(path);
		std::filesystem::create_directory(path / "Assets");

		Project newProject;
		newProject.m_Title = path.stem().string();
		newProject.m_ProjectDirectory = path;
		newProject.m_AssetsDirectory = newProject.m_ProjectDirectory / "Assets";

		s_ActiveProject = newProject;

		s_ActiveProject.Save();

		LOG_INFO("Created new project '{}' at {}", s_ActiveProject.m_Title, s_ActiveProject.GetProjectDirectory().string());
		return s_ActiveProject;
	}

	void Project::Load(const std::filesystem::path& path)
	{
		std::string configFilename = path.filename().string() + ".json"; // "TestProject.json"
		std::filesystem::path configPath = path / configFilename;

		//std::filesystem::path configPath = path / "project.json";

		if (!std::filesystem::exists(configPath))
		{
			LOG_ERROR("Config file not found at {}", configPath);
			return;
		}

		ProjectConfigJSON data;
		std::string buffer;

		if (auto ec = glz::read_file_json(data, configPath.string(), buffer))
		{
			LOG_ERROR("Failed to parse JSON in {}: {}", configPath, glz::format_error(ec, buffer));
			return;
		}

		Project project;
		project.m_ProjectDirectory = path;
		project.m_AssetsDirectory = path / "Assets";
		project.m_Title = data.Title;
		project.m_DefaultSceneId = Engine::Uuid(data.StartScene);

		LOG_INFO("Loaded project '{}' from {}", project.m_Title, configPath);
		s_ActiveProject = project;
	}

	Project& Project::GetActive()
	{
		return s_ActiveProject;
	}

	void Project::Save()
	{
		ProjectConfigJSON outData;
		outData.Title = this->m_Title;
		outData.StartScene = (uint64_t)this->m_DefaultSceneId;

		std::string configFilename = this->m_ProjectDirectory.filename().string() + ".json"; // "TestProject.json"
		std::filesystem::path path = this->m_ProjectDirectory / configFilename;
		//std::filesystem::path path = this->m_ProjectDirectory / "project.json";
		std::string buffer;

		if (auto ec = glz::write_file_json(outData, path.string(), buffer))
		{
			LOG_ERROR("Failed to save project to {}: {}", path, glz::format_error(ec));
			return;
		}

		LOG_INFO("Saved project to {}", path);
	}
}