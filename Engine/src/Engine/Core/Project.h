#pragma once

#include "Engine/Asset/EditorAssetManager.h"
#include <string>
#include <filesystem>

namespace Engine
{
	class Project
	{
	public:
		Ref<EditorAssetManager> GetAssetManager() { return m_AssetManager; }

		static Ref<Project> GetActive() { return s_ActiveProject; }


		inline static std::filesystem::path GetProjectDirectory()
		{
			return s_ActiveProject->m_ProjectDirectory;
		}

		inline static std::filesystem::path GetAssetsDirectory()
		{
			return s_ActiveProject->m_ProjectDirectory / "Assets";
		}

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& projectDirectoryPath);
		static void Save();

	private:
		std::filesystem::path m_ProjectDirectory;
		std::string m_Title = "Untitled";
		UUID m_StartSceneID = UUID(0);
		Ref<EditorAssetManager> m_AssetManager;

		inline static Ref<Project> s_ActiveProject;
	};
}