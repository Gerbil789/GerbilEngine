#pragma once

#include "Engine/Core/API.h"
#include "Engine/Graphics/Texture.h" //TODO: remove include from header
#include <webgpu/webgpu.hpp>

namespace Engine
{
	class Texture2D;
	class CubeMapTexture;
	struct AssetRecord;

	class ENGINE_API TextureImporter
	{
	public:
		static Texture2D* ImportTexture2D(const AssetRecord& record);
		static Texture2D* LoadTexture2D(const std::filesystem::path& path, TextureSpecification spec = {});

		static wgpu::TextureView LoadTexture2DWithMipMaps(const std::vector<std::filesystem::path>& paths);

		static CubeMapTexture* ImportCubeMapTexture(const AssetRecord& record);
		static CubeMapTexture* LoadCubeMapTexture(const std::filesystem::path& path);
	};
}