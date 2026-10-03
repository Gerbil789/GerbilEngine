#include "enginepch.h"
#include "Engine/Asset/Importer/ShaderImporter.h"
#include "Engine/Utility/File.h"

namespace engine
{
	ShaderAsset ShaderImporter::LoadShader(const std::filesystem::path& path)
	{
		std::string source;
		if (!engine::ReadFile(path, source))
		{
			LOG_ERROR("Failed to read shader file at '{}'", path);
			return {};
		}

		auto shader = ShaderAsset(source);
		return shader;
	}
}