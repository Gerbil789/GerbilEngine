#pragma once

#include "Engine/Graphics/Material.h"
#include "Engine/Core/Scene.h"
#include <filesystem>

namespace engine
{
	struct Serializer
	{
		static void SerializeMaterial(Material material, const std::filesystem::path& path);
		static MaterialAsset DeserializeMaterial(const std::filesystem::path& path);

		static void SerializeMaterialInstance(MaterialInstance materialInstance, const std::filesystem::path& path);
		static MaterialInstanceData DeserializeMaterialInstance(const std::filesystem::path& path);

		static void SerializeScene(Scene scene, const std::filesystem::path& path);
		static SceneAsset DeserializeScene(const std::filesystem::path& path);
	};
}