#pragma once

#include "Engine/Core/API.h"
#include <filesystem>

namespace Engine 
{
	bool ENGINE_API ReadFile(const std::filesystem::path& path, std::string& outData);
}