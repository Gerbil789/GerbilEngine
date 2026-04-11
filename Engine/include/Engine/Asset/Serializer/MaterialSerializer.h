#pragma once

namespace Engine
{
	class Material;

	class ENGINE_API MaterialSerializer
	{
	public:
		static void Serialize(Material* material, const std::filesystem::path& path);
		static Material* Deserialize(const std::filesystem::path& path);
	};
}