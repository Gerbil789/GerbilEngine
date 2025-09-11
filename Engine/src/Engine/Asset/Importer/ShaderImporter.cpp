#include "enginepch.h"
#include "ShaderImporter.h"
#include "Engine/Core/Project.h"

namespace Engine
{
	Ref<Shader> MaterialImporter::ImportShader(const AssetMetadata& metadata)
	{
		return LoadShader(Project::GetAssetsDirectory() / metadata.path);
	}

	Ref<Shader> MaterialImporter::LoadShader(const std::filesystem::path& path)
	{
		return Ref<Shader>();
	}
}