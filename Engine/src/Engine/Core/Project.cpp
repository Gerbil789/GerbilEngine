#include "enginepch.h"
#include "Project.h"
#include "Engine/Utils/File.h"

namespace Engine
{
	Ref<Project> Project::New()
	{
		auto path = Engine::OpenDirectory();
		if (path.empty())
		{
			LOG_WARNING("Project::New - No path selected for new project.");
			return nullptr;
		}

		s_ActiveProject = CreateRef<Project>();
		s_ActiveProject->m_AssetManager = CreateRef<EditorAssetManager>();
		s_ActiveProject->m_AssetManager->Initialize();
		s_ActiveProject->m_ProjectDirectory = path;
		return s_ActiveProject;
	}

	Ref<Project> Project::Load(const std::filesystem::path& path)
	{
		Ref<Project> project = CreateRef<Project>();

		//ProjectSerializer serializer(project);
		//if (serializer.Deserialize(path))
		//{
		//	project->m_ProjectDirectory = path.parent_path();
		//	s_ActiveProject = project;
		//	std::shared_ptr<EditorAssetManager> editorAssetManager = std::make_shared<EditorAssetManager>();
		//	s_ActiveProject->m_AssetManager = editorAssetManager;
		//	editorAssetManager->DeserializeAssetRegistry();
		//	return s_ActiveProject;
		//}

		return nullptr;
	}

	bool Project::SaveActive(const std::filesystem::path& path)
	{
		//ProjectSerializer serializer(s_ActiveProject);
		//if (serializer.Serialize(path))
		//{
		//	s_ActiveProject->m_ProjectDirectory = path.parent_path();
		//	return true;
		//}

		return false;
	}
}