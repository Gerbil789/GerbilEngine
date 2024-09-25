#include "enginepch.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Core/Serializer.h"

namespace Engine
{


	Ref<Asset> MaterialFactory::Load(const std::filesystem::path& path, const std::any& data)
	{
		Ref<Material> material = CreateRef<Material>(path);
		if (!Serializer::Deserialize(material))
		{
			LOG_ERROR("Failed to deserialize material");
		}
		return material;
	}

	Ref<Asset> MaterialFactory::Create(const std::filesystem::path& path, const std::any& data)
	{
		Ref<Material> material = CreateRef<Material>(path);
		material->shader = AssetManager::GetAsset<Shader>("resources/shaders/standard.glsl");
		return material;
	}
}