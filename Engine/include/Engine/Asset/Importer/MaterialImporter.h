#pragma once

namespace Engine
{
	class Material;

	class MaterialImporter
	{
	public:
		static std::optional<Material> LoadMaterial(const std::filesystem::path& path);
	};
}