#pragma once

#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Renderer/Shaders/Shader.h"

namespace Engine
{
	class MaterialImporter
	{
	public:
		static Ref<Shader> ImportShader(const AssetMetadata& metadata);
		static Ref<Shader> LoadShader(const std::filesystem::path& path);
	};
}