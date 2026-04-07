#pragma once

#include "Engine/Core/API.h"

namespace Engine
{
	class Material;

	class MaterialImporter
	{
	public:
		static Material* ImportMaterial(const std::filesystem::path& path);
		static Material* LoadMaterial(const std::filesystem::path& path);
	};
}