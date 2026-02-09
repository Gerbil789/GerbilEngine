#pragma once

#include "Engine/Core/API.h"
#include "Engine/Core/EngineContext.h"
#include <filesystem>

namespace Engine
{
	ENGINE_API void InitializeEngine(const EngineContext& context);
	ENGINE_API const std::filesystem::path& GetAssetsDirectory();
	ENGINE_API const std::filesystem::path& GetProjectDirectory();
}