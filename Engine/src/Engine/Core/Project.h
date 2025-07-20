#pragma once

#include <string>
#include <filesystem>

namespace Engine
{
	class Project
	{
	public:
		Project() = default;
		Project(const std::filesystem::path& path)
		{
			m_Path = std::filesystem::absolute(path);
			m_Title = m_Path.lexically_normal().filename().string();
			std::filesystem::create_directories(m_Path / "Assets");	// Ensure Assets directory exists
		}

		std::string GetTitle() const { return m_Title; }
		std::filesystem::path GetPath() const { return m_Path; }
		std::filesystem::path GetAssetsPath() const { return m_Path / "Assets"; }

	private:
		std::string m_Title;
		std::filesystem::path m_Path;

	};
}
