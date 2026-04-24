#include "enginepch.h"
#include "Engine/Asset/Importer/ShaderImporter.h"
#include "Engine/Asset/Serializer/ShaderParser.h"
#include "Engine/Utility/File.h"

namespace Engine
{
	std::optional<Shader> ShaderImporter::LoadShader(const std::filesystem::path& path)
	{
		std::string source;
		if (!Engine::ReadFile(path, source))
		{
			LOG_ERROR("Failed to read shader file. %s", path);
			return std::nullopt;
		}

		ShaderSpecification spec = ShaderParser::GetSpecification(source);
		auto shader = Shader(spec, source, path.stem().string());
		return shader;
	}
}