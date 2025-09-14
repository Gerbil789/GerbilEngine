#include "enginepch.h"
#include "ShaderImporter.h"
#include "Engine/Core/Project.h"
#include "Engine/Asset/Serializer/ShaderSerializer.h"
#include "Engine/Utils/File.h"

namespace Engine
{
	Ref<Shader> ShaderImporter::ImportShader(const AssetMetadata& metadata)
	{
		return LoadShader(Project::GetAssetsDirectory() / metadata.path);
	}

	Ref<Shader> ShaderImporter::LoadShader(const std::filesystem::path& path)
	{
		std::string content;
		if (!Engine::ReadFile(path, content))
		{
			LOG_ERROR("Failed to load shader. %s", path);
			return nullptr;
		}

		size_t sepPos = content.find("#SHADER");
		if (sepPos == std::string::npos)
		{
			LOG_ERROR("Shader file has no metadata separator '---'");
			return nullptr;
		}

		std::string metadata = content.substr(0, sepPos);
		std::string source = content.substr(sepPos + 7);

		ShaderSpecification spec = ShaderSerializer::DeserializeMetadata(metadata, path.stem().string());

		auto shader = CreateRef<Shader>(spec, source);

		return shader;
	}
}