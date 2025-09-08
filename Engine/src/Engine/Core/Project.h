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
		static bool SaveActive(const std::filesystem::path& path);

		inline static std::filesystem::path GetProjectDirectory()
		{
			return s_ActiveProject->m_ProjectDirectory;
		}

		inline static std::filesystem::path GetAssetsDirectory()
		{
			return s_ActiveProject->m_ProjectDirectory / "Assets";
		}

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& path);

	private:
		std::filesystem::path m_ProjectDirectory;
		Ref<EditorAssetManager> m_AssetManager;

		inline static Ref<Project> s_ActiveProject;
	};
}