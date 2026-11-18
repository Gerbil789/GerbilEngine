#pragma once

#include "engine/graphics/material.h"
#include "engine/core/scene.h"
#include <filesystem>

namespace engine
{
	struct serializer
	{
		static void SerializeMaterial(MaterialHandle material, const std::filesystem::path& path);
		static Material DeserializeMaterial(const std::filesystem::path& path);

		static void SerializeMaterialInstance(MaterialInstanceHandle materialInstance, const std::filesystem::path& path);
		static MaterialInstance DeserializeMaterialInstance(const std::filesystem::path& path);

		static void SerializeScene(SceneHandle handle, const std::filesystem::path& path);
		static Scene DeserializeScene(const std::filesystem::path& path);
	};
}