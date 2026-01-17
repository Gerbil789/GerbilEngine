#include "enginepch.h"
#include "ShaderImporter.h"
#include "Engine/Core/Project.h"
#include "Engine/Asset/Serializer/ShaderParser.h"
#include "Engine/Utils/File.h"

namespace Engine
{
	Shader* ShaderImporter::ImportShader(const AssetRecord& metadata)
	{
		return LoadShader(Project::GetAssetsDirectory() / metadata.path);
	}

	Shader* ShaderImporter::LoadShader(const std::filesystem::path& path)
	{
		std::string source;
		if (!Engine::ReadFile(path, source))
		{
			LOG_ERROR("Failed to read shader file. %s", path);
			return nullptr;
		}

		ShaderSpecification spec = ShaderParser::GetSpecification(source);
		auto shader = new Shader(spec, source, path.stem().string());
		return shader;
	}
}