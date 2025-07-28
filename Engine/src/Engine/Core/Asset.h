#pragma once

#include <filesystem>

namespace Engine
{
	class IAsset
	{
	public:
		virtual ~IAsset() = default;
		const std::filesystem::path& GetPath() const { return m_Path; }
	protected:
		IAsset(const std::filesystem::path& path) : m_Path(path) {}
		std::filesystem::path m_Path;
	};
}