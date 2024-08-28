#include "enginepch.h"
#include "Engine/Scene/Material.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Core/Serializer.h"

namespace Engine
{
	Ref<Asset> MaterialFactory::Load(const std::string& filePath)
	{
		Ref<Material> material = CreateRef<Material>(filePath);
		if (!Serializer::Deserialize(material))
		{
			ENGINE_LOG_ERROR("Failed to deserialize material");
		}
		return material;
	}

	Ref<Asset> MaterialFactory::Create(const std::string& filePath)
	{
		Ref<Material> material = CreateRef<Material>(filePath);
		return material;
	}
}