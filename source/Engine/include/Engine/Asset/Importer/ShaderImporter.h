#pragma once

#include "Engine/Graphics/Shader.h"

namespace Engine
{
	class ShaderImporter
	{
	public:
		static std::optional<ShaderAsset> LoadShader(const std::filesystem::path& path);
	};
}