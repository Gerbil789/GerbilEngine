#pragma once

#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Texture.h"
#include <glm/glm.hpp>

namespace Engine 
{
	class Material
	{
	public:
		Material() = default;
		

		//shader
		std::string shaderName = "Texture";

		//shader data (uniforms, textures)
		glm::vec4 color = glm::vec4(1.0f);
		Ref<Texture2D> texture;
		glm::vec2 tiling = glm::vec2(1.0f);

		//flags (blending, depth testing, two-sided, ...)

	};

}