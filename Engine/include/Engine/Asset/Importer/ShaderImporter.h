#pragma once

namespace Engine
{
	class Shader;

	class ShaderImporter
	{
	public:
		static Shader* ImportShader(const std::filesystem::path& path);
		static Shader* LoadShader(const std::filesystem::path& path);
	};
}