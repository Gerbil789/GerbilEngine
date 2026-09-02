#pragma once

#include "Engine/Asset/AssetType.h"
#include "Engine/Core/UUID.h"
#include <filesystem>

namespace engine
{
	struct AssetRecord
	{
		Uuid id;
		std::filesystem::path path;
		AssetType type;

		std::string GetName() const { return path.stem().string(); } //TODO: remove, especial dont call in hotpaths!!!

		operator bool() const { return id; }
	};
};