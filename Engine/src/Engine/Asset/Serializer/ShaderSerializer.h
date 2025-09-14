#pragma once

#include "Engine/Renderer/Shader.h"

namespace Engine
{
	class ShaderSerializer
	{
	public:
		static ShaderSpecification DeserializeMetadata(const std::string& data, const std::string& shaderName);
	};
}