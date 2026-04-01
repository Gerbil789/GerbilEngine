#include "enginepch.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/SamplerPool.h"
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Graphics/WebGPUUtils.h"
#include "Engine/Graphics/Texture.h"
#include "Engine/Core/Engine.h"

namespace Engine::RenderGlobals
{
	// Shadow
	ShadowUniforms s_ShadowUniforms;
	static std::array<wgpu::TextureView, shadowCascadeCount> m_DepthTextureViews;
	static wgpu::TextureView m_DepthTextureArrayView;

	// Model bind group
	static wgpu::BindGroupLayout s_ModelBindGroupLayout = nullptr;
	static wgpu::BindGroup s_ModelBindGroup = nullptr;
	static wgpu::Buffer s_ModelUniformBuffer = nullptr;

	// Frame bind group
	static wgpu::BindGroupLayout s_FrameBindGroupLayout = nullptr;
	static wgpu::BindGroup s_FrameBindGroup = nullptr;
	static wgpu::Buffer s_FrameUniformBuffer = nullptr;
	static wgpu::Buffer s_ShadowUniformBuffer = nullptr;


	struct alignas(16) ModelUniforms
	{
		glm::mat4 modelMatrix;
	};
	static_assert(sizeof(ModelUniforms) % 16 == 0);


	static uint32_t CeilToNextMultiple(uint32_t value, uint32_t step)
	{
		uint32_t divide_and_ceil = value / step + (value % step == 0 ? 0 : 1);
		return step * divide_and_ceil;
	}


	void Initialize()
	{
		wgpu::Device device = GraphicsContext::GetDevice();

		wgpu::Limits limits;
		if (device.getLimits(&limits) != wgpu::Status::Success)
		{
			throw std::runtime_error("Failed to get device limits");
		}

		UniformStride = limits.minUniformBufferOffsetAlignment;
		StorageStride = limits.minStorageBufferOffsetAlignment;


		// Shadow
		{
			wgpu::TextureFormat format = wgpu::TextureFormat::Depth24Plus;

			wgpu::TextureDescriptor textureDesc;
			textureDesc.dimension = wgpu::TextureDimension::_2D;
			textureDesc.format = format;
			textureDesc.mipLevelCount = 1;
			textureDesc.sampleCount = 1;
			textureDesc.size = { 1024, 1024, RenderGlobals::shadowCascadeCount };
			textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc;
			textureDesc.viewFormatCount = 0;
			textureDesc.viewFormats = nullptr;
			wgpu::Texture texture = GraphicsContext::GetDevice().createTexture(textureDesc);

			for (int i = 0; i < RenderGlobals::shadowCascadeCount; i++)
			{
				wgpu::TextureViewDescriptor viewDesc;
				viewDesc.format = format;
				viewDesc.aspect = wgpu::TextureAspect::DepthOnly;
				viewDesc.dimension = wgpu::TextureViewDimension::_2D;
				viewDesc.baseMipLevel = 0;
				viewDesc.mipLevelCount = 1;
				viewDesc.baseArrayLayer = i;
				viewDesc.arrayLayerCount = 1;

				m_DepthTextureViews[i] = texture.createView(viewDesc);
			}

			wgpu::TextureViewDescriptor arrayViewDesc;
			arrayViewDesc.format = format;
			arrayViewDesc.aspect = wgpu::TextureAspect::DepthOnly;
			arrayViewDesc.dimension = wgpu::TextureViewDimension::_2DArray;
			arrayViewDesc.baseMipLevel = 0;
			arrayViewDesc.mipLevelCount = 1;
			arrayViewDesc.baseArrayLayer = 0;
			arrayViewDesc.arrayLayerCount = RenderGlobals::shadowCascadeCount;

			m_DepthTextureArrayView = texture.createView(arrayViewDesc);
		}

		// Model 
		{
			const size_t BufferSize = 1024 * UniformStride; // max 1024 unique transforms per frame //TODO: make this configurable or dynamic

			wgpu::BindGroupLayoutEntry bindGroupLayoutEntry = wgpu::Default;
			bindGroupLayoutEntry.binding = 0;
			bindGroupLayoutEntry.visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
			bindGroupLayoutEntry.buffer.hasDynamicOffset = true;
			bindGroupLayoutEntry.buffer.type = wgpu::BufferBindingType::Uniform;
			bindGroupLayoutEntry.buffer.minBindingSize = sizeof(ModelUniforms);

			wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc{};
			bindGroupLayoutDesc.label = { "ModelBindGroupLayout", WGPU_STRLEN };
			bindGroupLayoutDesc.entryCount = 1;
			bindGroupLayoutDesc.entries = &bindGroupLayoutEntry;
			s_ModelBindGroupLayout = device.createBindGroupLayout(bindGroupLayoutDesc);

			wgpu::BufferDescriptor bufferDesc{};
			bufferDesc.label = { "ModelUniformBuffer", WGPU_STRLEN };
			bufferDesc.size = BufferSize;
			bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
			s_ModelUniformBuffer = device.createBuffer(bufferDesc);

			wgpu::BindGroupEntry bindGroupEntry{};
			bindGroupEntry.binding = 0;
			bindGroupEntry.buffer = s_ModelUniformBuffer;
			bindGroupEntry.offset = 0;
			bindGroupEntry.size = sizeof(ModelUniforms);

			wgpu::BindGroupDescriptor bindGroupDesc{};
			bindGroupDesc.label = { "ModelBindGroup", WGPU_STRLEN };
			bindGroupDesc.layout = s_ModelBindGroupLayout;
			bindGroupDesc.entryCount = 1;
			bindGroupDesc.entries = &bindGroupEntry;
			s_ModelBindGroup = device.createBindGroup(bindGroupDesc);
		}

		// Frame 
		{
			std::array<wgpu::BindGroupLayoutEntry, 8> entries;

			// Frame uniforms
			entries[0].binding = 0;
			entries[0].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
			entries[0].buffer.type = wgpu::BufferBindingType::Uniform;
			entries[0].buffer.minBindingSize = sizeof(FrameUniforms);

			// Environment sampler
			entries[1].binding = 1;
			entries[1].visibility = wgpu::ShaderStage::Fragment;
			entries[1].sampler.type = wgpu::SamplerBindingType::Filtering;

			// Irradiance texture
			entries[2].binding = 2;
			entries[2].visibility = wgpu::ShaderStage::Fragment;
			entries[2].texture.sampleType = wgpu::TextureSampleType::Float;
			entries[2].texture.viewDimension = wgpu::TextureViewDimension::_2D;
			entries[2].texture.multisampled = false;

			// BRDF integration texture
			entries[3].binding = 3;
			entries[3].visibility = wgpu::ShaderStage::Fragment;
			entries[3].texture.sampleType = wgpu::TextureSampleType::Float;
			entries[3].texture.viewDimension = wgpu::TextureViewDimension::_2D;
			entries[3].texture.multisampled = false;

			// Prefiltered environment texture
			entries[4].binding = 4;
			entries[4].visibility = wgpu::ShaderStage::Fragment;
			entries[4].texture.sampleType = wgpu::TextureSampleType::Float;
			entries[4].texture.viewDimension = wgpu::TextureViewDimension::_2D;
			entries[4].texture.multisampled = false;

			// Shadow map texture
			entries[5].binding = 5;
			entries[5].visibility = wgpu::ShaderStage::Fragment;
			entries[5].texture.sampleType = wgpu::TextureSampleType::Depth;
			entries[5].texture.viewDimension = wgpu::TextureViewDimension::_2DArray;
			entries[5].texture.multisampled = false;

			// Shadow map sampler
			entries[6].binding = 6;
			entries[6].visibility = wgpu::ShaderStage::Fragment;
			entries[6].sampler.type = wgpu::SamplerBindingType::Comparison;

			// Shadow uniforms
			entries[7].binding = 7;
			entries[7].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
			entries[7].buffer.type = wgpu::BufferBindingType::Uniform;
			entries[7].buffer.minBindingSize = sizeof(ShadowUniforms);


			wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
			bindGroupLayoutDesc.label = { "FrameBindGroupLayout", WGPU_STRLEN };
			bindGroupLayoutDesc.entryCount = entries.size();
			bindGroupLayoutDesc.entries = entries.data();

			s_FrameBindGroupLayout = device.createBindGroupLayout(bindGroupLayoutDesc);

			wgpu::BufferDescriptor bufferDesc{};
			bufferDesc.label = { "FrameUniformBuffer", WGPU_STRLEN };
			bufferDesc.size = sizeof(FrameUniforms);
			bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;

			s_FrameUniformBuffer = device.createBuffer(bufferDesc);

			std::array<wgpu::BindGroupEntry, 8> bgEntries;

			bgEntries[0].binding = 0;
			bgEntries[0].buffer = s_FrameUniformBuffer;
			bgEntries[0].offset = 0;
			bgEntries[0].size = sizeof(FrameUniforms);

			wgpu::SamplerDescriptor envSamplerDesc;
			envSamplerDesc.label = { "EnvironmentSampler", WGPU_STRLEN };
			envSamplerDesc.minFilter = wgpu::FilterMode::Linear;
			envSamplerDesc.magFilter = wgpu::FilterMode::Linear;
			envSamplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
			envSamplerDesc.maxAnisotropy = 1;

			wgpu::Sampler envSampler = device.createSampler(envSamplerDesc);
			bgEntries[1].binding = 1;
			bgEntries[1].sampler = envSampler;

			auto irrTexture = TextureImporter::LoadTexture2D("Resources/Engine/hdr/PG2/lebombo_irradiance_map.hdr");
			bgEntries[2].binding = 2;
			bgEntries[2].textureView = irrTexture->GetTextureView();

			auto brdfTexture = TextureImporter::LoadTexture2D("Resources/Engine/hdr/PG2/brdf_integration_map_ct_ggx.hdr");
			bgEntries[3].binding = 3;
			bgEntries[3].textureView = brdfTexture->GetTextureView();

			std::vector<std::filesystem::path> paths
			{
				"Resources/Engine/hdr/PG2/lebombo_prefiltered_env_map_001.hdr",
				"Resources/Engine/hdr/PG2/lebombo_prefiltered_env_map_125.hdr",
				"Resources/Engine/hdr/PG2/lebombo_prefiltered_env_map_250.hdr",
				"Resources/Engine/hdr/PG2/lebombo_prefiltered_env_map_375.hdr",
				"Resources/Engine/hdr/PG2/lebombo_prefiltered_env_map_500.hdr",
				"Resources/Engine/hdr/PG2/lebombo_prefiltered_env_map_625.hdr",
				"Resources/Engine/hdr/PG2/lebombo_prefiltered_env_map_750.hdr",
				"Resources/Engine/hdr/PG2/lebombo_prefiltered_env_map_875.hdr",
				"Resources/Engine/hdr/PG2/lebombo_prefiltered_env_map_999.hdr"
			};

			auto view = TextureImporter::LoadTexture2DWithMipMaps(paths);
			bgEntries[4].binding = 4;
			bgEntries[4].textureView = view;


			bgEntries[5].binding = 5;
			bgEntries[5].textureView = m_DepthTextureArrayView;

			wgpu::SamplerDescriptor desc;
			desc.label = { "ShadowSampler", WGPU_STRLEN };
			desc.compare = wgpu::CompareFunction::LessEqual;
			desc.minFilter = wgpu::FilterMode::Linear;
			desc.magFilter = wgpu::FilterMode::Linear;
			desc.maxAnisotropy = 1;

			wgpu::Sampler shadowSampler = device.createSampler(desc);

			bgEntries[6].binding = 6;
			bgEntries[6].sampler = shadowSampler;

			wgpu::BufferDescriptor shadowBufferDesc;
			shadowBufferDesc.label = { "ShadowUniformBuffer", WGPU_STRLEN };
			shadowBufferDesc.size = sizeof(ShadowUniforms);
			shadowBufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
			s_ShadowUniformBuffer = device.createBuffer(shadowBufferDesc);

			bgEntries[7].binding = 7;
			bgEntries[7].buffer = s_ShadowUniformBuffer;
			bgEntries[7].offset = 0;
			bgEntries[7].size = sizeof(ShadowUniforms);

			wgpu::BindGroupDescriptor bindGroupDesc;
			bindGroupDesc.label = { "FrameBindGroup", WGPU_STRLEN };
			bindGroupDesc.layout = s_FrameBindGroupLayout;
			bindGroupDesc.entryCount = bgEntries.size();
			bindGroupDesc.entries = bgEntries.data();
			s_FrameBindGroup = device.createBindGroup(bindGroupDesc);
		}
	}

	wgpu::BindGroupLayout GetFrameLayout()
	{
		return s_FrameBindGroupLayout;

	}

	wgpu::BindGroup GetFrameBindGroup()
	{
		return s_FrameBindGroup;
	}

	wgpu::Buffer GetFrameUniformBuffer()
	{
		return s_FrameUniformBuffer;
	}

	wgpu::TextureView GetShadowTextureView(uint32_t cascadeIndex)
	{
		assert(cascadeIndex < shadowCascadeCount);
		return m_DepthTextureViews[cascadeIndex];
	}

	wgpu::Buffer GetShadowUniformBuffer()
	{
		return s_ShadowUniformBuffer;
	}

	wgpu::BindGroupLayout GetModelLayout()
	{
		return s_ModelBindGroupLayout;
	}

	wgpu::BindGroup GetModelBindGroup()
	{
		return s_ModelBindGroup;
	}

	wgpu::Buffer GetModelUniformBuffer()
	{
		return s_ModelUniformBuffer;
	}
}