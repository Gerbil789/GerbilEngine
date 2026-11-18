#pragma once

#include "engine/graphics/shader.h"
#include "engine/graphics/pipeline.h"
#include "engine/graphics/sampler_pool.h"
#include "engine/graphics/texture/texture_2d.h"
#include "engine/core/log.h"
#include <glm/glm.hpp>

namespace engine
{
	//TODO: move away...
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

		void SettextureFilter(TextureFilter filter);
		void SettextureWrap(TextureWrap wrap);

		void Settexture(uint32_t bindingIndex, TextureHandle texture);
		TextureHandle Gettexture(uint32_t bindingIndex) const;

		wgpu::RenderPipeline GetPipeline() const;
	};
}