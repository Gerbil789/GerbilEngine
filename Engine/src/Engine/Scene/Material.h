#pragma once

#include "Engine/Core/Asset.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Engine 
{
	enum class SurfaceType { Opaque, Transparent };

	class Material : public Asset
	{
	public:
		Material() = default;

		virtual void Load(const std::string& filePath) override;
		virtual void Unload() override;


		std::string shaderName = "Texture"; //todo: change to shader

		SurfaceType surfaceType = SurfaceType::Opaque;

		Ref<Texture2D> colorTexture;
		glm::vec4 color = glm::vec4(1.0f);

		Ref<Texture2D> metallicTexture;
		float metallic = 0.0f;

		Ref<Texture2D> roughnessTexture;
		float roughness = 0.0f;

		Ref<Texture2D> normalTexture;
		float normalStrength = 1.0f;

		glm::vec2 tiling = glm::vec2(1.0f);
		glm::vec2 offset = glm::vec2(0.0f);
	};
}