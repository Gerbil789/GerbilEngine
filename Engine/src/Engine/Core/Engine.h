#pragma once

#include "Engine/Core/EngineContext.h"
#include <filesystem>

namespace Engine
{
	void InitializeEngine(const EngineContext& context);

	const std::filesystem::path& GetAssetsDirectory();
	const std::filesystem::path& GetProjectDirectory();
}