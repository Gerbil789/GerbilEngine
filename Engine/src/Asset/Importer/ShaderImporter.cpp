#include "enginepch.h"
#include "Engine/Asset/Importer/ShaderImporter.h"
#include "Engine/Asset/Serializer/ShaderParser.h"
#include "Engine/Utility/File.h"

namespace Engine
{
	Shader* ShaderImporter::ImportShader(const std::filesystem::path& path)
	{
		return LoadShader(path);
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