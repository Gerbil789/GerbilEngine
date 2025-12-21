#pragma once

#include "Engine/Renderer/Material.h"

namespace Engine
{
	class MaterialSerializer
	{
	public:
		static void Serialize(Material* material, const std::filesystem::path& path);
		static Material* Deserialize(const std::filesystem::path& path);
	};
}