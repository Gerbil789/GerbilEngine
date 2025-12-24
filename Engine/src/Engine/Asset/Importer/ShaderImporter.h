#pragma once

#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Graphics/Shader.h"

namespace Engine
{
	class ShaderImporter
	{
	public:
		static Shader* ImportShader(const AssetMetadata& metadata);
		static Shader* LoadShader(const std::filesystem::path& path);
	};
}