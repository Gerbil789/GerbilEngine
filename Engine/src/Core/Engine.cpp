#include "enginepch.h"
#include "Engine/Core/Engine.h"

namespace Engine
{
	static EngineContext s_Context;

	void InitializeEngine(const EngineContext& context)
	{
		s_Context = context;
	}

	const std::filesystem::path& GetAssetsDirectory()
	{
		return s_Context.AssetsDirectory;
	}

	const std::filesystem::path& GetProjectDirectory()
	{
		return s_Context.ProjectDirectory;
	}
}