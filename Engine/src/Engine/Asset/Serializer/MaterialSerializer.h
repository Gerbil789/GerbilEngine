#pragma once

#include "Engine/Renderer/Material.h"

namespace Engine
{
	class MaterialSerializer
	{
	public:
		static void Serialize(const Ref<Material>& material, const std::filesystem::path& path);
		static Material* Deserialize(const std::filesystem::path& path);
	};
}