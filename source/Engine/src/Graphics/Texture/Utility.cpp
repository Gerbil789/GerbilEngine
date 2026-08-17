#include "enginepch.h"
#include "Engine/Graphics/Texture/Utility.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Utility.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Texture/TextureCube.h"
#include "Engine/Core/Assert.h"
#include <bit>

namespace Engine
{
	uint32_t GetMaxMipLevelCount(const wgpu::Extent3D& textureSize)
	{
		return std::bit_width(std::max(textureSize.width, textureSize.height));
	}

	//TODO: cache the pipeline and layouts for cubemap creation
	static wgpu::BindGroupLayout CreateBindGroupLayout(wgpu::TextureFormat format)
	{
		std::array<wgpu::BindGroupLayoutEntry, 3> bindings;
		bindings[0].binding = 0;
		bindings[0].sampler.type = wgpu::SamplerBindingType::Filtering;
		bindings[0].visibility = wgpu::ShaderStage::Compute;

		bindings[1].binding = 1;
		bindings[1].texture.sampleType = wgpu::TextureSampleType::Float;
		bindings[1].texture.viewDimension = wgpu::TextureViewDimension::e2D;
		bindings[1].visibility = wgpu::ShaderStage::Compute;

		bindings[2].binding = 2;
		bindings[2].storageTexture.access = wgpu::StorageTextureAccess::WriteOnly;
		bindings[2].storageTexture.format = format;
		bindings[2].storageTexture.viewDimension = wgpu::TextureViewDimension::e2DArray;
		bindings[2].visibility = wgpu::ShaderStage::Compute;

		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
		bindGroupLayoutDesc.entryCount = bindings.size();
		bindGroupLayoutDesc.entries = bindings.data();
		wgpu::BindGroupLayout bindGroupLayout = GraphicsContext::GetDevice().CreateBindGroupLayout(&bindGroupLayoutDesc);

		return bindGroupLayout;
	}

	static wgpu::ComputePipeline CreateComputePipeline(wgpu::BindGroupLayout layout)
	{
		wgpu::ShaderModule computeShaderModule = LoadWGSLShader("resources/shaders/compute/cubemap.wgsl");

		wgpu::PipelineLayoutDescriptor pipelineLayoutDesc;
		pipelineLayoutDesc.bindGroupLayoutCount = 1;
		pipelineLayoutDesc.bindGroupLayouts = &layout;
		wgpu::PipelineLayout pipelineLayout = GraphicsContext::GetDevice().CreatePipelineLayout(&pipelineLayoutDesc);

		wgpu::ComputePipelineDescriptor computePipelineDesc;
		computePipelineDesc.compute.entryPoint = "equirectToCubemap";
		computePipelineDesc.compute.module = computeShaderModule;
		computePipelineDesc.layout = pipelineLayout;
		wgpu::ComputePipeline computePipeline = GraphicsContext::GetDevice().CreateComputePipeline(&computePipelineDesc);

		return computePipeline;
	}

	static wgpu::Sampler CreateSampler()
	{
		wgpu::SamplerDescriptor samplerDesc;
		samplerDesc.magFilter = wgpu::FilterMode::Linear;
		samplerDesc.minFilter = wgpu::FilterMode::Linear;
		samplerDesc.maxAnisotropy = 1;
		wgpu::Sampler linearSampler = GraphicsContext::GetDevice().CreateSampler(&samplerDesc);
		return linearSampler;
	}

	static wgpu::BindGroup CreateBindGroup(wgpu::Sampler sampler, wgpu::TextureView sourceView, wgpu::TextureView targetView, wgpu::BindGroupLayout layout)
	{
		std::array<wgpu::BindGroupEntry, 3> entries;
		entries[0].binding = 0;
		entries[0].sampler = sampler;
		entries[1].binding = 1;
		entries[1].textureView = sourceView;
		entries[2].binding = 2;
		entries[2].textureView = targetView; // Write to our new cubemap array view

		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.layout = layout;
		bindGroupDesc.entryCount = entries.size();
		bindGroupDesc.entries = entries.data();
		wgpu::BindGroup bindGroup = GraphicsContext::GetDevice().CreateBindGroup(&bindGroupDesc);

		return bindGroup;
	}

	TextureCube EquirectangularToCubemap(Uuid equirectangularTexture)
	{
		const Texture2D& sourceTexture = Engine::AssetManager::GetAsset<Texture2D>(equirectangularTexture);

		ENGINE_ASSERT(sourceTexture.GetWidth() == sourceTexture.GetHeight() * 2, "Equirectangular texture must have a 2:1 aspect ratio.");

		TextureCubeSpecification spec;
		spec.size = sourceTexture.GetHeight() / 2;
		spec.format = sourceTexture.GetFormat();
		spec.mipCount = 1;
		TextureCube textureCube(spec);

		// view for WRITING to the 6 faces of the target cubemap (Mip Level 0)
		wgpu::TextureViewDescriptor targetViewDesc;
		targetViewDesc.label = "CubemapTargetWriteView";
		targetViewDesc.format = spec.format;
		targetViewDesc.dimension = wgpu::TextureViewDimension::e2DArray;
		targetViewDesc.baseMipLevel = 0;      // write to the top mip level
		targetViewDesc.mipLevelCount = 1;
		targetViewDesc.baseArrayLayer = 0;
		targetViewDesc.arrayLayerCount = 6;   // write to all 6 faces
		targetViewDesc.aspect = wgpu::TextureAspect::All;
		wgpu::TextureView targetWriteView = textureCube.GetTexture().CreateView(&targetViewDesc);

		wgpu::BindGroupLayout bindGroupLayout = CreateBindGroupLayout(spec.format);
		wgpu::ComputePipeline pipeline = CreateComputePipeline(bindGroupLayout);
		wgpu::Sampler linearSampler = CreateSampler();
		wgpu::BindGroup bindGroup = CreateBindGroup(linearSampler, sourceTexture.GetTextureView(), targetWriteView, bindGroupLayout);

		wgpu::CommandEncoder encoder = Engine::GraphicsContext::GetDevice().CreateCommandEncoder();

		wgpu::ComputePassDescriptor computePassDesc;
		computePassDesc.timestampWrites = nullptr;
		wgpu::ComputePassEncoder computePass = encoder.BeginComputePass(&computePassDesc);

		computePass.SetPipeline(pipeline);
		computePass.SetBindGroup(0, bindGroup, 0, nullptr);

		uint32_t workgroupSizePerDim = 4; // Matches WGSL @workgroup_size(4, 4, 1)
		uint32_t workgroupCountX = (spec.size + workgroupSizePerDim - 1) / workgroupSizePerDim;
		uint32_t workgroupCountY = (spec.size + workgroupSizePerDim - 1) / workgroupSizePerDim;

		computePass.DispatchWorkgroups(workgroupCountX, workgroupCountY, 6);
		computePass.End();

		wgpu::CommandBuffer commandBuffer = encoder.Finish();
		Engine::GraphicsContext::GetQueue().Submit(1, &commandBuffer);

		return textureCube;
	}

	void GenerateMipmaps(wgpu::Texture texture)
	{
		std::vector<wgpu::TextureView> textureMipViews;
		std::vector<wgpu::Extent3D> textureMipSizes;

		wgpu::TextureFormat format = texture.GetFormat();

		wgpu::Extent3D baseSize = { texture.GetWidth(), texture.GetHeight(), 1 };
		auto mipCount = GetMaxMipLevelCount(baseSize);

		wgpu::TextureViewDescriptor textureViewDesc;
		textureViewDesc.nextInChain = nullptr;
		textureViewDesc.aspect = wgpu::TextureAspect::All;
		textureViewDesc.baseArrayLayer = 0;
		textureViewDesc.arrayLayerCount = 6;
		textureViewDesc.dimension = wgpu::TextureViewDimension::e2DArray;
		textureViewDesc.format = format;
		textureViewDesc.mipLevelCount = 1;


		textureMipSizes.resize(mipCount);
		textureMipViews.reserve(mipCount);

		textureMipSizes[0] = baseSize;

		for (uint32_t level = 0; level < mipCount; ++level)
		{
			std::string label = "MIP level #" + std::to_string(level);
			textureViewDesc.label = label.c_str();
			textureViewDesc.baseMipLevel = level;
			textureMipViews.push_back(texture.CreateView(&textureViewDesc));

			if (level > 0)
			{
				wgpu::Extent3D previousSize = textureMipSizes[level - 1];
				//textureMipSizes[level] = { previousSize.width / 2, previousSize.height / 2, 6 };

				textureMipSizes[level] = {std::max(1u, previousSize.width / 2), std::max(1u, previousSize.height / 2), 6};
			}
		}


		wgpu::ShaderModule computeShaderModule = LoadWGSLShader("resources/shaders/compute/mipmap.wgsl");

		// Create bind group layout
		std::array<wgpu::BindGroupLayoutEntry, 2> bindings;

		bindings[0].binding = 0;
		bindings[0].texture.sampleType = wgpu::TextureSampleType::Float;
		bindings[0].texture.viewDimension = wgpu::TextureViewDimension::e2DArray;
		bindings[0].visibility = wgpu::ShaderStage::Compute;

		bindings[1].binding = 1;
		bindings[1].storageTexture.access = wgpu::StorageTextureAccess::WriteOnly;
		bindings[1].storageTexture.format = format;
		bindings[1].storageTexture.viewDimension = wgpu::TextureViewDimension::e2DArray;
		bindings[1].visibility = wgpu::ShaderStage::Compute;

		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
		bindGroupLayoutDesc.entryCount = bindings.size();
		bindGroupLayoutDesc.entries = bindings.data();
		wgpu::BindGroupLayout bindGroupLayout = GraphicsContext::GetDevice().CreateBindGroupLayout(&bindGroupLayoutDesc);

		// Create compute pipeline layout
		wgpu::PipelineLayoutDescriptor pipelineLayoutDesc;
		pipelineLayoutDesc.bindGroupLayoutCount = 1;
		pipelineLayoutDesc.bindGroupLayouts = &bindGroupLayout;
		wgpu::PipelineLayout pipelineLayout = GraphicsContext::GetDevice().CreatePipelineLayout(&pipelineLayoutDesc);

		// Create compute pipeline;
		wgpu::ComputePipelineDescriptor computePipelineDesc;
		computePipelineDesc.compute.entryPoint = "computeMipMap";
		computePipelineDesc.compute.module = computeShaderModule;
		computePipelineDesc.layout = pipelineLayout;
		wgpu::ComputePipeline computeMipmapPipeline = GraphicsContext::GetDevice().CreateComputePipeline(&computePipelineDesc);


		auto CreateBindGroup = [&](uint32_t nextLevel)
			{
				std::array<wgpu::BindGroupEntry, 2> entries;

				entries[0].binding = 0;
				entries[0].textureView = textureMipViews[nextLevel - 1];

				entries[1].binding = 1;
				entries[1].textureView = textureMipViews[nextLevel];

				wgpu::BindGroupDescriptor bindGroupDesc;
				bindGroupDesc.layout = bindGroupLayout;
				bindGroupDesc.entryCount = (uint32_t)entries.size();
				bindGroupDesc.entries = entries.data();
				wgpu::BindGroup bindGroup = GraphicsContext::GetDevice().CreateBindGroup(&bindGroupDesc);
				return bindGroup;
			};

		auto encoder = Engine::GraphicsContext::GetDevice().CreateCommandEncoder();

		wgpu::ComputePassDescriptor computePassDesc;
		computePassDesc.timestampWrites = nullptr;
		wgpu::ComputePassEncoder computePass = encoder.BeginComputePass(&computePassDesc);

		computePass.SetPipeline(computeMipmapPipeline);

		for (uint32_t nextLevel = 1; nextLevel < mipCount; ++nextLevel)
		{
			computePass.SetBindGroup(0, CreateBindGroup(nextLevel), 0, nullptr);

			uint32_t invocationCountX = textureMipSizes[nextLevel].width;
			uint32_t invocationCountY = textureMipSizes[nextLevel].height;
			uint32_t workgroupSizePerDim = 8;
			uint32_t workgroupCountX = (invocationCountX + workgroupSizePerDim - 1) / workgroupSizePerDim;
			uint32_t workgroupCountY = (invocationCountY + workgroupSizePerDim - 1) / workgroupSizePerDim;
			computePass.DispatchWorkgroups(workgroupCountX, workgroupCountY, 6);
		}

		computePass.End();

		wgpu::CommandBuffer commandBuffer = encoder.Finish();
		Engine::GraphicsContext::GetQueue().Submit(1, &commandBuffer);
	}
}