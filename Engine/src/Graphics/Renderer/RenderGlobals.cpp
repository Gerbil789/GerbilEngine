#include "enginepch.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/SamplerPool.h"
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Graphics/Texture.h"
#include "Engine/Core/Engine.h"

namespace Engine::RenderGlobals
{
	// Model bind group
	static wgpu::BindGroupLayout s_ModelBindGroupLayout = nullptr;
	static wgpu::BindGroup s_ModelBindGroup = nullptr;
	static wgpu::Buffer s_ModelUniformBuffer = nullptr;
	static uint32_t s_ModelUniformStride = 0;

	// Frame bind group
	static wgpu::BindGroupLayout s_FrameBindGroupLayout = nullptr;
	static wgpu::BindGroup s_FrameBindGroup = nullptr;
	static wgpu::Buffer s_FrameUniformBuffer = nullptr;

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

		// Model 
		{
			wgpu::Limits limits;
			device.getLimits(&limits);
			s_ModelUniformStride = CeilToNextMultiple((uint32_t)sizeof(ModelUniforms), (uint32_t)limits.minUniformBufferOffsetAlignment);

			// stride is probably 256 on most hardware, but could be different on some

			const size_t BufferSize = 1024 * s_ModelUniformStride; // max 1024 unique transforms per frame

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
			wgpu::BindGroupLayoutEntry entries[13]{};

			// Frame uniforms
			entries[0].binding = 0;
			entries[0].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
			entries[0].buffer.type = wgpu::BufferBindingType::Uniform;
			entries[0].buffer.minBindingSize = sizeof(FrameUniforms);

			// Environment sampler
			entries[1].binding = 1;
			entries[1].visibility = wgpu::ShaderStage::Fragment;
			entries[1].sampler.type = wgpu::SamplerBindingType::NonFiltering;

			// Irradiance texture
			entries[2].binding = 2;
			entries[2].visibility = wgpu::ShaderStage::Fragment;
			entries[2].texture.sampleType = wgpu::TextureSampleType::UnfilterableFloat;
			entries[2].texture.viewDimension = wgpu::TextureViewDimension::_2D;
			entries[2].texture.multisampled = false;

			// BRDF integration texture
			entries[3].binding = 3;
			entries[3].visibility = wgpu::ShaderStage::Fragment;
			entries[3].texture.sampleType = wgpu::TextureSampleType::UnfilterableFloat;
			entries[3].texture.viewDimension = wgpu::TextureViewDimension::_2D;
			entries[3].texture.multisampled = false;

			// Prefiltered environment texture
			//entries[4].binding = 4;
			//entries[4].visibility = wgpu::ShaderStage::Fragment;
			//entries[4].texture.sampleType = wgpu::TextureSampleType::UnfilterableFloat;
			//entries[4].texture.viewDimension = wgpu::TextureViewDimension::_2D;
			//entries[4].texture.multisampled = false;

			// Prefiltered environment textures
			for (uint32_t i = 0; i < 9; i++)
			{
				entries[4 + i].binding = 4 + i;
				entries[4 + i].visibility = wgpu::ShaderStage::Fragment;
				entries[4 + i].texture.sampleType = wgpu::TextureSampleType::UnfilterableFloat;
				entries[4 + i].texture.viewDimension = wgpu::TextureViewDimension::_2D;
				entries[4 + i].texture.multisampled = false;
			}

			wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc{};
			bindGroupLayoutDesc.label = { "FrameBindGroupLayout", WGPU_STRLEN };
			bindGroupLayoutDesc.entryCount = 13;
			bindGroupLayoutDesc.entries = entries;

			s_FrameBindGroupLayout = device.createBindGroupLayout(bindGroupLayoutDesc);

			wgpu::BufferDescriptor bufferDesc{};
			bufferDesc.label = { "FrameUniformBuffer", WGPU_STRLEN };
			bufferDesc.size = sizeof(FrameUniforms);
			bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;

			s_FrameUniformBuffer = device.createBuffer(bufferDesc);

			wgpu::BindGroupEntry bgEntries[13]{};

			bgEntries[0].binding = 0;
			bgEntries[0].buffer = s_FrameUniformBuffer;
			bgEntries[0].offset = 0;
			bgEntries[0].size = sizeof(FrameUniforms);

			bgEntries[1].binding = 1;
			bgEntries[1].sampler = SamplerPool::GetSampler(TextureFilter::Point, TextureWrap::Clamp);

			auto irrTexture = TextureImporter::LoadTexture2D("Resources/Engine/hdr/PG2/lebombo_irradiance_map.hdr");
			bgEntries[2].binding = 2;
			bgEntries[2].textureView = irrTexture->GetTextureView();

			auto brdfTexture = TextureImporter::LoadTexture2D("Resources/Engine/hdr/PG2/brdf_integration_map_ct_ggx.hdr");
			bgEntries[3].binding = 3;
			bgEntries[3].textureView = brdfTexture->GetTextureView();

			//auto prefEnvTexture = TextureImporter::LoadTexture2D("Resources/Engine/hdr/PG2/lebombo_prefiltered_env_map_001.hdr");
			//bgEntries[4].binding = 4;
			//bgEntries[4].textureView = prefEnvTexture->GetTextureView();

			std::array<std::string, 9> paths
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

			for (uint32_t i = 0; i < 9; i++)
			{
				auto tex = TextureImporter::LoadTexture2D(paths[i]);
				bgEntries[4 + i].binding = 4 + i;
				bgEntries[4 + i].textureView = tex->GetTextureView();
			}


			wgpu::BindGroupDescriptor bindGroupDesc{};
			bindGroupDesc.label = { "FrameBindGroup", WGPU_STRLEN };
			bindGroupDesc.layout = s_FrameBindGroupLayout;
			bindGroupDesc.entryCount = 13;
			bindGroupDesc.entries = bgEntries;
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

	uint32_t GetModelUniformStride()
	{
		return s_ModelUniformStride;
	}
}