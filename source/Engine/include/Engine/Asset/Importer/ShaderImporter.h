#pragma once

#include "Engine/Graphics/Shader.h"

namespace engine
{
	class ShaderImporter
	{
	public:
		static Shader LoadShader(const std::filesystem::path& path);
	};
}