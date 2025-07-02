#include "enginepch.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Renderer.h"

namespace Engine 
{
	Ref<Asset> ShaderFactory::Load(const std::filesystem::path& path, const std::any& data)
	{
		return std::make_shared<Shader>(path);
	}
}