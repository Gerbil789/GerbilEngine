#pragma once

#include "Engine/Asset/AssetHandle.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Graphics/Pipeline.h"
#include "Engine/Graphics/SamplerPool.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Core/Log.h"
#include <glm/glm.hpp>

namespace engine
{
	struct alignas(16) PBRParams
	{
		alignas(16) glm::vec4 albedo;
		alignas(4) float roughness;
		alignas(4) float metallic;
		alignas(8) glm::vec2 tiling;
		alignas(8) glm::vec2 offset;
	};

	struct alignas(16) FlatParams
	{
		alignas(16) glm::vec4 albedo;
	};

	struct alignas(16) GridParams
	{
		alignas(16) glm::vec4 albedo;
		alignas(8) glm::vec2 tiling;
		alignas(8) glm::vec2 offset;
		alignas(8) glm::vec2 hoveredTile;
	};

	struct MaterialAsset
	{
		MaterialAsset() = default;
		MaterialAsset(Shader shader);

		Shader shader;
		wgpu::RenderPipeline pipeline;
		std::vector<std::byte> defaultUniformData;
	};

	struct Material : public AssetHandle
	{
		using AssetHandle::AssetHandle;
		MaterialAsset& Get() const;

		Shader GetShader() const;
		wgpu::RenderPipeline GetPipeline() const;
		size_t GetExpectedSize() const;
	};

	struct MaterialInstanceData
	{
		MaterialInstanceData() = default;
		MaterialInstanceData(Material material);

		Material material;

		wgpu::BindGroup bindGroup;
		wgpu::Buffer uniformBuffer;

		std::unordered_map<uint32_t, Texture2D> textures;
		std::vector<std::byte> uniformData;

		TextureFilter textureFilter = TextureFilter::Bilinear;
		TextureWrap textureWrap = TextureWrap::Repeat;

		void CreateBindGroup();
	};

	struct MaterialInstance : public AssetHandle
	{
		using AssetHandle::AssetHandle;
		MaterialInstanceData& Get() const;

		const Shader GetShader() const;

		template<typename T>
    void SetUniformData(const T& data)
    {
      MaterialInstanceData& instanceData = Get();
      
      if (sizeof(T) != instanceData.material.GetExpectedSize())
      {
        LOG_ERROR("Struct size mismatch!");
        return;
      }

      std::memcpy(instanceData.uniformData.data(), &data, sizeof(T));
    }

		void SetTextureFilter(TextureFilter filter);
		void SetTextureWrap(TextureWrap wrap);

		TextureFilter GetTextureFilter() const;
		TextureWrap GetTextureWrap() const;

		void SetTexture(uint32_t bindingIndex, Texture2D texture);
		Texture2D GetTexture(uint32_t bindingIndex) const;

		wgpu::BindGroup GetBindGroup() const;
		wgpu::Buffer GetUniformBuffer() const;
		const std::vector<std::byte>& GetUniformData() const;
		const std::unordered_map<uint32_t, Texture2D>& GetTextures() const;

		wgpu::RenderPipeline GetPipeline() const;
	};
}