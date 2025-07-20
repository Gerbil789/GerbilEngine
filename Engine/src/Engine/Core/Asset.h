#pragma once

#include <Engine/Core/Core.h>
#include <filesystem>

namespace Engine
{
	class Asset
	{
	public:
		virtual ~Asset() = default;
		const std::filesystem::path& GetFilePath() const { return m_Path; }
		void SetFilePath(const std::filesystem::path& path) { this->m_Path = path; }
		void SetModified(bool modified) { this->m_Modified = modified; } //TODO: i dont like this
		bool IsModified() const { return m_Modified; }
	protected:
		Asset(const std::filesystem::path& path) : m_Path(path) {}
		std::filesystem::path m_Path;
		bool m_Modified = false;
	};
}