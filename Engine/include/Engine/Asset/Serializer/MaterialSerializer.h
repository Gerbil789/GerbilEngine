#pragma once

#include "Engine/Graphics/Material.h"
#include <optional>

namespace Engine
{
	class ENGINE_API MaterialSerializer
	{
	public:
		static void Serialize(const Material& material, const std::filesystem::path& path);
		static std::optional<Material> Deserialize(const std::filesystem::path& path);
	};
}