#include "enginepch.h"
#include "Engine/Graphics/Texture/TextureCube.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Texture/Utility.h"
#include "Engine/Graphics/GraphicsContext.h"

#include "Engine/Graphics/WebGPUUtils.h"


namespace Engine
{
	TextureCube::TextureCube(const TextureSpecification& specification)
	{
		m_Width = specification.width;
		m_Height = specification.height;
		m_TextureFormat = specification.format;
		uint32_t faceSize = m_Height / 2;
		uint32_t mipCount = 1;

		if (specification.generateMips)
		{
			mipCount = GetMaxMipLevelCount({ faceSize, faceSize, 1 });
		}


		wgpu::TextureDescriptor textureDesc;
		textureDesc.dimension = wgpu::TextureDimension::_2D;
		textureDesc.format = m_TextureFormat;
		textureDesc.sampleCount = 1;
		textureDesc.viewFormatCount = 0;
		textureDesc.viewFormats = nullptr;
		textureDesc.size = { faceSize, faceSize, 6 };
		textureDesc.mipLevelCount = mipCount;
		textureDesc.usage = wgpu::TextureUsage::StorageBinding | wgpu::TextureUsage::CopySrc | wgpu::TextureUsage::TextureBinding;
		m_Texture = GraphicsContext::GetDevice().createTexture(textureDesc);

		wgpu::TextureViewDescriptor viewDesc;
		viewDesc.format = m_TextureFormat;
		viewDesc.aspect = wgpu::TextureAspect::All;
		viewDesc.dimension = wgpu::TextureViewDimension::Cube;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 6;
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = mipCount;
		m_TextureView = m_Texture.createView(viewDesc);
	}

	TextureCube::TextureCube(const TextureSpecification& specification, const void* data)
	{
		m_Width = specification.width;
		m_Height = specification.height;
		m_TextureFormat = specification.format;

		wgpu::TextureDescriptor textureDesc;
		textureDesc.dimension = wgpu::TextureDimension::_2D;
		textureDesc.format = m_TextureFormat;
		textureDesc.mipLevelCount = 1;
		textureDesc.sampleCount = 1;
		textureDesc.size = { m_Width, m_Height, 1 };
		textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
		textureDesc.viewFormatCount = 0;
		textureDesc.viewFormats = nullptr;
		wgpu::Texture inputTexture = GraphicsContext::GetDevice().createTexture(textureDesc);

		uint32_t bytesPerPixel = 4;
		if (m_TextureFormat == wgpu::TextureFormat::RGBA16Float)
		{
			bytesPerPixel = 8;
		}

		wgpu::TexelCopyTextureInfo dst;
		dst.texture = inputTexture;
		dst.mipLevel = 0;
		dst.origin = { 0, 0, 0 };
		dst.aspect = wgpu::TextureAspect::All;

		wgpu::TexelCopyBufferLayout layout;
		layout.offset = 0;
		layout.bytesPerRow = m_Width * bytesPerPixel;
		layout.rowsPerImage = m_Height;

		wgpu::Extent3D size = { m_Width, m_Height, 1 };

		GraphicsContext::GetQueue().writeTexture(dst, data, m_Width * m_Height * bytesPerPixel, layout, size);


		uint32_t faceSize = m_Height / 2;
		uint32_t mipCount = GetMaxMipLevelCount({ faceSize, faceSize, 1 });

		textureDesc.size = { faceSize, faceSize, 6 };
		textureDesc.mipLevelCount = mipCount;
		textureDesc.usage = wgpu::TextureUsage::StorageBinding | wgpu::TextureUsage::CopySrc | wgpu::TextureUsage::TextureBinding;
		m_Texture = GraphicsContext::GetDevice().createTexture(textureDesc);

		std::vector<wgpu::TextureView> m_cubemapTextureMips;

		wgpu::TextureViewDescriptor viewDesc;
		viewDesc.format = m_TextureFormat;
		viewDesc.dimension = wgpu::TextureViewDimension::_2DArray;
		viewDesc.mipLevelCount = 1;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 6;
		viewDesc.aspect = wgpu::TextureAspect::All;
		m_cubemapTextureMips.resize(mipCount, nullptr);
		for (uint32_t level = 0; level < mipCount; ++level)
		{
			viewDesc.baseMipLevel = level;
			m_cubemapTextureMips[level] = m_Texture.createView(viewDesc);
		}

		viewDesc.arrayLayerCount = 1;
		viewDesc.baseMipLevel = 0;
		viewDesc.dimension = wgpu::TextureViewDimension::_2D;
		wgpu::TextureView inputTextureView = inputTexture.createView(viewDesc);


		viewDesc.dimension = wgpu::TextureViewDimension::Cube;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 6;
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = mipCount;
		m_TextureView = m_Texture.createView(viewDesc);

		wgpu::ShaderModule computeShaderModule = LoadWGSLShader("Resources/Engine/shaders/compute/cubemap.wgsl");

		// Create bind group layout
		std::vector<wgpu::BindGroupLayoutEntry> bindings(3, wgpu::Default);

		bindings[0].binding = 0;
		bindings[0].sampler.type = wgpu::SamplerBindingType::Filtering;
		bindings[0].visibility = wgpu::ShaderStage::Compute;

		bindings[1].binding = 1;
		bindings[1].texture.sampleType = wgpu::TextureSampleType::Float;
		bindings[1].texture.viewDimension = wgpu::TextureViewDimension::_2D;
		bindings[1].visibility = wgpu::ShaderStage::Compute;

		bindings[2].binding = 2;
		bindings[2].storageTexture.access = wgpu::StorageTextureAccess::WriteOnly;
		bindings[2].storageTexture.format = m_TextureFormat;
		bindings[2].storageTexture.viewDimension = wgpu::TextureViewDimension::_2DArray;
		bindings[2].visibility = wgpu::ShaderStage::Compute;

		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
		bindGroupLayoutDesc.entryCount = bindings.size();
		bindGroupLayoutDesc.entries = bindings.data();
		wgpu::BindGroupLayout bindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(bindGroupLayoutDesc);

		// Create compute pipeline layout
		wgpu::PipelineLayoutDescriptor pipelineLayoutDesc;
		pipelineLayoutDesc.bindGroupLayoutCount = 1;
		pipelineLayoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)&bindGroupLayout;
		wgpu::PipelineLayout pipelineLayout = GraphicsContext::GetDevice().createPipelineLayout(pipelineLayoutDesc);

		// Create compute pipeline;
		wgpu::ComputePipelineDescriptor computePipelineDesc = wgpu::Default;
		computePipelineDesc.compute.entryPoint = { "equirectToCubemap", WGPU_STRLEN };
		computePipelineDesc.compute.module = computeShaderModule;
		computePipelineDesc.layout = pipelineLayout;
		wgpu::ComputePipeline computePipeline = GraphicsContext::GetDevice().createComputePipeline(computePipelineDesc);


		std::vector<wgpu::BindGroup> m_bindGroups;

		m_bindGroups.resize(mipCount, nullptr);

		std::vector<wgpu::BindGroupEntry> entries(3, wgpu::Default);

		entries[0].binding = 0;
		entries[0].sampler = GraphicsContext::GetDevice().createSampler();

		entries[1].binding = 1;
		entries[1].textureView = inputTextureView;

		entries[2].binding = 2;
		entries[2].textureView = m_cubemapTextureMips[0];

		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.layout = bindGroupLayout;
		bindGroupDesc.entryCount = (uint32_t)entries.size();
		bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
		wgpu::BindGroup bindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);

		auto encoder = Engine::GraphicsContext::GetDevice().createCommandEncoder();

		wgpu::ComputePassDescriptor computePassDesc;
		computePassDesc.timestampWrites = nullptr;
		wgpu::ComputePassEncoder computePass = encoder.beginComputePass(computePassDesc);

		computePass.setPipeline(computePipeline);


		computePass.setBindGroup(0, bindGroup, 0, nullptr);

		uint32_t invocationCountX = faceSize;
		uint32_t invocationCountY = faceSize;
		uint32_t workgroupSizePerDim = 4;

		uint32_t workgroupCountX = (invocationCountX + workgroupSizePerDim - 1) / workgroupSizePerDim;
		uint32_t workgroupCountY = (invocationCountY + workgroupSizePerDim - 1) / workgroupSizePerDim;
		computePass.dispatchWorkgroups(workgroupCountX, workgroupCountY, 6);
		computePass.end();

		wgpu::CommandBuffer commandBuffer = encoder.finish();
		Engine::GraphicsContext::GetQueue().submit(1, &commandBuffer);




		//create preview view
		wgpu::TextureViewDescriptor previewViewDesc;
		previewViewDesc.label = { "CubemapPreviewFace0", WGPU_STRLEN };
		previewViewDesc.format = m_TextureFormat;

		// IMPORTANT: Treat this view as a standard 2D texture, NOT a Cube or 2DArray
		previewViewDesc.dimension = wgpu::TextureViewDimension::_2D;

		// Just grab Mip 0
		previewViewDesc.baseMipLevel = 0;
		previewViewDesc.mipLevelCount = 1;

		// Grab exactly one face. 
		// 0 = +X (Right), 1 = -X (Left), 2 = +Y (Top), 3 = -Y (Bottom), 4 = +Z (Front), 5 = -Z (Back)
		previewViewDesc.baseArrayLayer = 0;
		previewViewDesc.arrayLayerCount = 1; // Only 1 layer!

		previewViewDesc.aspect = wgpu::TextureAspect::All;

		m_PreviewTextureView = m_Texture.createView(previewViewDesc);


		ImportanceSample(m_Texture);

		//GenerateMipmaps(m_Texture);
	}
}