#pragma once

#include "Engine/Graphics/Material.h"
#include "Engine/Core/Scene.h"
#include <filesystem>

namespace engine
{
	struct Serializer
	{
		static void SerializeMaterial(MaterialHandle material, const std::filesystem::path& path);
		static Material DeserializeMaterial(const std::filesystem::path& path);

		static void SerializeMaterialInstance(MaterialInstanceHandle materialInstance, const std::filesystem::path& path);
		static MaterialInstance DeserializeMaterialInstance(const std::filesystem::path& path);

		static void SerializeScene(SceneHandle scene, const std::filesystem::path& path);
		static Scene DeserializeScene(const std::filesystem::path& path);
	};
}