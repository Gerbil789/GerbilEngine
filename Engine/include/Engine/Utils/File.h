#pragma once

#include "Engine/Core/API.h"
#include <string>
#include <filesystem>

namespace Engine 
{
	bool ENGINE_API ReadFile(const std::filesystem::path& path, std::string& outData);

	std::string ENGINE_API OpenFile();
	std::string	ENGINE_API SaveFile();

	std::string ENGINE_API OpenDirectory();

	void ENGINE_API OpenFileExplorer(const std::filesystem::path& path);
}