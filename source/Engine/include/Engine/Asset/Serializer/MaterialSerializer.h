#pragma once

#include "Engine/Graphics/Material.h"
#include <optional>
#include <filesystem>

namespace engine
{
	class MaterialSerializer
	{
	public:
		static void Serialize(Material material, const std::filesystem::path& path);
		static std::optional<MaterialAsset> Deserialize(const std::filesystem::path& path);
	};
}