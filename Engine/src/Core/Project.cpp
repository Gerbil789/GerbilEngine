#include "enginepch.h"
#include "Engine/Core/Project.h"
#include "Engine/Core/Log.h"
#include <yaml-cpp/yaml.h>
#include <fstream>

namespace Engine
{
	Project Project::s_ActiveProject;

	Project* Project::New(const std::string& title, const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			LOG_ERROR("Project::New - Path does not exist: {}", path);
			return nullptr;
		}

		std::filesystem::create_directory(path / title);
		std::filesystem::create_directory(path / title / "Assets");

		Project* newProject = new Project();
		newProject->m_Title = title;
		newProject->m_ProjectDirectory = path / title;
		newProject->m_AssetsDirectory = newProject->m_ProjectDirectory / "Assets";
		newProject->Save();

		LOG_INFO("Created new project '{}' at {}", title, newProject->GetProjectDirectory());
		return newProject;
	}

	void Project::Load(const std::filesystem::path& path)
	{
		std::filesystem::path configPath = path / "project.yaml";

		if (!std::filesystem::exists(configPath))
		{
			LOG_ERROR("Config file not found at {}", configPath);
			return;
		}

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(configPath.string());
		}
		catch (const YAML::ParserException& e)
		{
			LOG_ERROR("Failed to parse YAML : {}", e.what());
			return;
		}

		Project project;
		project.m_ProjectDirectory = path;
		project.m_AssetsDirectory = path / "Assets";

		if (data["Title"])
		{
			project.m_Title = data["Title"].as<std::string>();
		}

		uint64_t id = data["StartScene"] ? data["StartScene"].as<uint64_t>(0) : 0;
		project.m_StartSceneID = Engine::Uuid(id);
		
		LOG_INFO("Loaded project '{}' from {}", project.m_Title, configPath);
		s_ActiveProject = project;
	}

	Project& Project::GetActive()
	{
		return s_ActiveProject;
	}

	void Project::Save()
	{
		//YAML::Emitter out;
		//out << YAML::BeginMap;
		//out << YAML::Key << "Title" << YAML::Value << this->m_Title;
		//out << YAML::Key << "StartScene" << YAML::Value << (uint64_t)this->m_StartSceneID;
		//out << YAML::EndMap;

		//std::filesystem::path path = this->m_ProjectDirectory / "project.yaml";
		//std::ofstream fout(path);
		//fout << out.c_str();

		//LOG_INFO("Saved project to {}", path);
	}


}