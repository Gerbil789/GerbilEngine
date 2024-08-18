#include "enginepch.h"
#include "Engine/Scene/Material.h"
#include "Engine/Core/AssetManager.h"

namespace Engine
{
	Ref<Asset> MaterialFactory::Create(const std::string& filePath)
	{
		Ref<Material> material = CreateRef<Material>();
		material->Load(filePath);
		return material;
	}

	void Material::Load(const std::string& filePath)
	{
		this->colorTexture = AssetManager::LoadAsset<Texture2D>("assets/textures/gerbil.jpg");
		this->metallic = 1.0f;
		this->filePath = filePath;
		isLoaded = true;
	}

	void Material::Unload()
	{
		isLoaded = false;
	}
}