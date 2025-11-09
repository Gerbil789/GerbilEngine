#pragma once

#include <string>
#include <filesystem>

namespace Engine
{
	class Project
	{
	public:
		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& projectDirectoryPath);
		static void Save();

		static Ref<Project> GetActive() { return s_ActiveProject; }

		inline static std::filesystem::path GetProjectDirectory()
		{
			return s_ActiveProject->m_ProjectDirectory;
		}

		inline static std::filesystem::path GetAssetsDirectory()
		{
			return s_ActiveProject->m_ProjectDirectory / "Assets";
		}

		inline static std::string GetTitle()
		{
			return s_ActiveProject->m_Title;
		}

		inline static UUID GetStartSceneID()
		{
			return s_ActiveProject->m_StartSceneID;
		}

	private:
		std::filesystem::path m_ProjectDirectory;
		std::string m_Title = "Untitled";
		UUID m_StartSceneID = UUID(0);

		inline static Ref<Project> s_ActiveProject;
	};
}