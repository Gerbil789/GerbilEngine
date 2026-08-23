#pragma once

#include "Engine/Asset/AssetHandle.h"
#include <filesystem>

namespace Engine
{
	class Project
	{
	public:
		static void Load(const std::filesystem::path& path);
		static Project& GetActive();

		void Save(); //TODO

		const std::filesystem::path& GetProjectDirectory() const { return m_ProjectDirectory; }
		const std::filesystem::path& GetAssetsDirectory() const { return m_AssetsDirectory; }
		std::filesystem::path& GetAssetsDirectory() { return m_AssetsDirectory; }
		const std::string& GetTitle() const { return m_Title; }
		Engine::Scene GetDefaultScene() const { return m_DefaultScene; } //TODO: rename it, default is not good name

	private:
		static Project s_ActiveProject;

		std::filesystem::path m_ProjectDirectory;
		std::filesystem::path m_AssetsDirectory;
		std::string m_Title = "Untitled";
		Engine::Scene m_DefaultScene;
	};
}