#include "enginepch.h"
#include "Project.h"
#include <yaml-cpp/yaml.h>

namespace Engine
{
	Ref<Project> Project::New()
	{
		LOG_ERROR("Project::New - Not implemented yet!");
		return nullptr;
	}

	Ref<Project> Project::Load(const std::filesystem::path& projectDirectoryPath)
	{
		std::filesystem::path configPath = projectDirectoryPath / "project.yaml";

		if (!std::filesystem::exists(configPath))
		{
			LOG_ERROR("Project::Load - Config file not found at {0}", configPath.string());
			return nullptr;
		}

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(configPath.string());
		}
		catch (const YAML::ParserException& e)
		{
			LOG_ERROR("Project::Load - Failed to parse YAML: {0}", e.what());
			return nullptr;
		}

		s_ActiveProject = CreateRef<Project>();
		s_ActiveProject->m_ProjectDirectory = projectDirectoryPath;

		if (data["Title"])
		{
			s_ActiveProject->m_Title = data["Title"].as<std::string>();
		}

		if (data["StartSceneID"])
		{
			uint64_t id = data["StartSceneID"].as<uint64_t>();
			s_ActiveProject->m_StartSceneID = UUID(id);
		}

		s_ActiveProject->m_ProjectDirectory = projectDirectoryPath;
		s_ActiveProject->m_AssetManager = CreateRef<EditorAssetManager>();
		s_ActiveProject->m_AssetManager->Initialize();

		LOG_INFO("Loaded project '{0}' from {1}", s_ActiveProject->m_Title, configPath);
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
		out << YAML::Key << "StartSceneID" << YAML::Value << (uint64_t)s_ActiveProject->m_StartSceneID;
		out << YAML::Key << "ProjectDirectory" << YAML::Value << s_ActiveProject->m_ProjectDirectory.string();
		out << YAML::EndMap;

		std::filesystem::path savePath = s_ActiveProject->m_ProjectDirectory / "project.yaml";
		std::ofstream fout(savePath);
		fout << out.c_str();

		LOG_INFO("Saved project to {0}", savePath.string());
	}
}