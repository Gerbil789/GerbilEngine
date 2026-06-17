#include "IconManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/Resources.h"
#include "Engine/Asset/AssetRecord.h"
#include <glm/glm.hpp>
#include <array>

namespace Editor
{
	namespace
	{
		constexpr size_t IconCount = static_cast<size_t>(Icon::Count);
		constexpr std::array<glm::ivec2, IconCount> IconCoords
		{
				glm::ivec2{0, 0}, // EmptyFolder
				glm::ivec2{1, 0}, // Folder
				glm::ivec2{2, 0}, // File
				glm::ivec2{3, 0}, // EmptyFile
				glm::ivec2{4, 0}, // Audio
				glm::ivec2{5, 0}, // Landscape
				glm::ivec2{6, 0}, // Image
				glm::ivec2{7, 0}, // Mesh
		};

		constexpr glm::ivec2 m_CellSize{ 64, 64 };
		std::vector<Engine::SubTexture2D> m_Icons;
	}

	void IconManager::Initialize()
	{
		m_Icons.reserve(IconCoords.size());
		for (size_t i = 0; i < IconCoords.size(); ++i)
		{
			m_Icons.emplace_back(Engine::SubTexture2D::CreateFromGrid(RESOURCES::TEXTURE::EDITOR_ICONS, IconCoords[i], m_CellSize));
		}
	}

	Engine::SubTexture2D& IconManager::GetIcon(Icon icon)
	{
		return m_Icons[static_cast<size_t>(icon)];
	}

	Engine::SubTexture2D& IconManager::GetIcon(Engine::AssetType assetType)
	{
		static constexpr std::array<std::pair<Engine::AssetType, Icon>, IconCount> map
		{
			std::pair{Engine::AssetType::Directory,				Icon::Folder},
			std::pair{Engine::AssetType::EmptyDirectory,	Icon::EmptyFolder},
			std::pair{Engine::AssetType::Texture2D,				Icon::Image},
			std::pair{Engine::AssetType::Scene,						Icon::Landscape},
			std::pair{Engine::AssetType::Material,				Icon::File},
			std::pair{Engine::AssetType::Mesh,						Icon::Mesh},
			std::pair{Engine::AssetType::Audio,						Icon::Audio}
		};

		for (const auto& [type, icon] : map)
		{
			if (assetType == type)
			{
				return GetIcon(icon);
			}
		}

		return GetIcon(Icon::File); // fallback, TODO: use unknown/invalid icon
	}
}