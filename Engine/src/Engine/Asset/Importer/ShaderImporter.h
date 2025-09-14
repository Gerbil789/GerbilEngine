#pragma once

#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Renderer/Shader.h"

namespace Engine
{
	class ShaderImporter
	{
	public:
		static Ref<Shader> ImportShader(const AssetMetadata& metadata);
		static Ref<Shader> LoadShader(const std::filesystem::path& path);
	};
}