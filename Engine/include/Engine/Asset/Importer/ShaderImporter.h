#pragma once

#include "Engine/Asset/AssetRecord.h"
#include "Engine/Graphics/Shader.h"

namespace Engine
{
	class ShaderImporter
	{
	public:
		static Shader* ImportShader(const AssetRecord& metadata);
		static Shader* LoadShader(const std::filesystem::path& path);
	};
}