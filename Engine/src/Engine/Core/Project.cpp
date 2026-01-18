#include "enginepch.h"
#include "Project.h"
#include <yaml-cpp/yaml.h>

namespace Engine
{
	Project* Project::New()
	{
		LOG_ERROR("Project::New - Not implemented yet!");
		return nullptr;
	}

	Project* Project::Load(const std::filesystem::path& path)
	{
		std::filesystem::path configPath = path / "project.yaml";

		if (!std::filesystem::exists(configPath))
		{
			LOG_ERROR("Project::Load - Config file not found at {}", configPath);
			return nullptr;
		}

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(configPath.string());
		}
		catch (const YAML::ParserException& e)
		{
			LOG_ERROR("Project::Load - Failed to parse YAML: {}", e.what());
			return nullptr;
		}

		s_ActiveProject = new Project();
		s_ActiveProject->m_ProjectDirectory = path;

		if (data["Title"])
		{
			s_ActiveProject->m_Title = data["Title"].as<std::string>();
		}

		uint64_t id = data["StartScene"] ? data["StartScene"].as<uint64_t>(0) : 0;
		s_ActiveProject->m_StartSceneID = UUID(id);

		LOG_INFO("Loaded project '{}' from {}", s_ActiveProject->m_Title, configPath);
		return s_ActiveProject;
	}

	void Project::Save()
	{
		if (!s_ActiveProject)
		{
			LOG_ERROR("Project::Save - No active project!");
			return;
		}

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Title" << YAML::Value << s_ActiveProject->m_Title;
		out << YAML::Key << "StartScene" << YAML::Value << (uint64_t)s_ActiveProject->m_StartSceneID;
		out << YAML::EndMap;

		std::filesystem::path savePath = s_ActiveProject->m_ProjectDirectory / "project.yaml";
		std::ofstream fout(savePath);
		fout << out.c_str();

		LOG_INFO("Saved project to {0}", savePath.string());
	}
}