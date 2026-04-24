#pragma once

namespace Engine
{
	class Shader;

	class ShaderImporter
	{
	public:
		static std::optional<Shader> LoadShader(const std::filesystem::path& path);
	};
}