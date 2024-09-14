#pragma once

#include "Engine/Core/Asset.h"

namespace Engine
{
	class MeshFactory : public IAssetFactory
	{
	public:
		virtual Ref<Asset> Load(const std::string& filePath) override;
		virtual Ref<Asset> Create(const std::string& filePath) override;
	};

	class Mesh : public Asset
	{
	public:
		Mesh(const std::string& filePath) : Asset(filePath) {}
	};
}