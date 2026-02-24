#pragma once

namespace Engine
{
	class Shader;
	struct AssetRecord;

	class ShaderImporter
	{
	public:
		static Shader* ImportShader(const AssetRecord& record);
		static Shader* LoadShader(const std::filesystem::path& path);
	};
}