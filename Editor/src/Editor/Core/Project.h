#pragma once

#include "Engine/Core/UUID.h"
#include <filesystem>

namespace Editor
{
	class Project
	{
	public:
		static Project* New(const std::string& title, const std::filesystem::path& path);
		static Project* Load(const std::filesystem::path& path);

		void Save();

		const std::filesystem::path& GetProjectDirectory() const { return m_ProjectDirectory; }
		const std::filesystem::path& GetAssetsDirectory() const { return m_AssetsDirectory; }
		const std::string& GetTitle() const { return m_Title; }
		const Engine::UUID& GetStartSceneID() const { return m_StartSceneID; }

	private:
		std::filesystem::path m_ProjectDirectory;
		std::filesystem::path m_AssetsDirectory;
		std::string m_Title = "Untitled";
		Engine::UUID m_StartSceneID = Engine::UUID{ 0 };
	};
}