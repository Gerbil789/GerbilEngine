#pragma once

#include "Engine/Core/Asset.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Texture.h"
#include <glm/glm.hpp>

namespace Engine 
{
	class MaterialFactory : public IAssetFactory
	{
	public:
		Ref<Asset> Load(const std::filesystem::path& path, const std::any& data = std::any()) override;
		Ref<Asset> Create(const std::filesystem::path& path, const std::any& data = std::any()) override;
	};

	class Material : public Asset
	{
	public:
		Material(const std::filesystem::path& path) : Asset(path) {}

		PROPERTY(Ref<Shader>, Shader);

		PROPERTY(Ref<Texture2D>, ColorTexture);
		PROPERTY(Ref<Texture2D>, MetallicTexture);
		PROPERTY(Ref<Texture2D>, RoughnessTexture);
		PROPERTY(Ref<Texture2D>, NormalTexture);
		PROPERTY(Ref<Texture2D>, HeightTexture);
		PROPERTY(Ref<Texture2D>, AmbientTexture);
		PROPERTY(Ref<Texture2D>, EmissionTexture);

		PROPERTY(glm::vec4, Color);
		PROPERTY(float, Metallic);
		PROPERTY(float, Roughness);
		PROPERTY(float, NormalStrength);
		PROPERTY(glm::vec3, EmissionColor);
		PROPERTY(float, EmmissionStrength);

		PROPERTY(glm::vec2, Tiling);
		PROPERTY(glm::vec2, Offset);
	};
}