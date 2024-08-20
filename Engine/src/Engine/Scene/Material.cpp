#include "enginepch.h"
#include "Engine/Scene/Material.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Core/Serializer.h"

namespace Engine
{
	Ref<Asset> MaterialFactory::Create(const std::string& filePath)
	{
		Ref<Material> material = CreateRef<Material>();
		if (!Serializer::Deserialize(filePath, material))
		{
			ENGINE_LOG_ERROR("Failed to deserialize material");
		}
		return material;
	}

	//void Material::Load(const std::string& filePath)
	//{
	//	/*Ref<Material> material = CreateRef<Material>();
	//	



	//	this->colorTexture = AssetManager::LoadAsset<Texture2D>("assets/textures/gerbil.jpg");
	//	this->metallic = 1.0f;
	//	this->filePath = filePath;
	//	SetName(filePath);
	//	isLoaded = true;*/
	//}

	//void Material::Unload()
	//{
	//	isLoaded = false;
	//}
}