#pragma once

#include "Engine/Graphics/Material.h"
#include <optional>
#include <filesystem>

namespace Engine
{
	class ENGINE_API MaterialSerializer
	{
	public:
		static void Serialize(Material material, const std::filesystem::path& path);
		static std::optional<MaterialAsset> Deserialize(const std::filesystem::path& path);
	};
}