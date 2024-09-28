#pragma once

#include "Engine/Core/Asset.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Texture.h"
#include <glm/glm.hpp>

namespace Engine 
{
	enum class SurfaceType { Opaque, Transparent };

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

		Ref<Shader> shader;

		Ref<Texture2D> colorTexture;
		Ref<Texture2D> metallicTexture;
		Ref<Texture2D> roughnessTexture;
		Ref<Texture2D> normalTexture;
		Ref<Texture2D> heightTexture;
		Ref<Texture2D> occlusionTexture;
		Ref<Texture2D> emissionTexture;

		glm::vec4 color = glm::vec4(1.0f);
		float metallic = 0.0f;
		float roughness = 0.0f;
		float normalStrength = 1.0f;
		glm::vec3 emissionColor = glm::vec3(0.0f);
		float emmissionStrength = 1.0f;

		glm::vec2 tiling = { 1.0f, 1.0f };
		glm::vec2 offset = { 0.0f, 0.0f };
	};
}