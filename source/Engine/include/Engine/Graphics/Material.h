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

	struct Material
	{
		Material() = default;
		Material(ShaderHandle shader);

		ShaderHandle shaderHandle;
		wgpu::RenderPipeline pipeline;
	};

	struct MaterialHandle : public AssetHandle
	{
		using AssetHandle::AssetHandle;
		Material& Get() const;

		static const MaterialHandle Pink;
		static const MaterialHandle White;
	};

	inline constexpr MaterialHandle MaterialHandle::Pink{ 0 };
	inline constexpr MaterialHandle MaterialHandle::White{ 1 };

	struct MaterialInstance
	{
		MaterialInstance() = default;
		MaterialInstance(MaterialHandle material);

		MaterialHandle materialHandle;

		wgpu::BindGroup bindGroup;
		wgpu::Buffer uniformBuffer;

		std::unordered_map<uint32_t, TextureHandle> textures;
		std::vector<std::byte> uniformData;

		TextureFilter textureFilter = TextureFilter::Bilinear;
		TextureWrap textureWrap = TextureWrap::Repeat;

		void CreateBindGroup();


		template<typename T>
		void SetUniformData(const T& data)
		{
			size_t bufferSize = materialHandle.Get().shaderHandle.Get().materialUniformBufferSize;

			if (sizeof(T) != bufferSize)
			{
				LOG_ERROR("Struct size mismatch!");
				return;
			}

			std::memcpy(uniformData.data(), &data, sizeof(T));
		}

		void SetTextureFilter(TextureFilter filter);
		void SetTextureWrap(TextureWrap wrap);

		void SetTexture(uint32_t bindingIndex, TextureHandle texture);
		TextureHandle GetTexture(uint32_t bindingIndex) const;

		wgpu::RenderPipeline GetPipeline() const;
	};

	struct MaterialInstanceHandle : public AssetHandle
	{
		using AssetHandle::AssetHandle;
		MaterialInstance& Get() const;

		static const MaterialInstanceHandle Pink;
		static const MaterialInstanceHandle White;
		static const MaterialInstanceHandle Null;
	};

	inline constexpr MaterialInstanceHandle MaterialInstanceHandle::Pink{ 0 };
	inline constexpr MaterialInstanceHandle MaterialInstanceHandle::White{ 1 };
	inline constexpr MaterialInstanceHandle MaterialInstanceHandle::Null{ ~0u };
}