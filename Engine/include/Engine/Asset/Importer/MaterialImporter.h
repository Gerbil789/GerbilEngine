#pragma once

#include "Engine/Core/API.h"

namespace Engine
{
	class Material;

	class MaterialImporter
	{
	public:
		static std::optional<Material> LoadMaterial(const std::filesystem::path& path);
	};
}