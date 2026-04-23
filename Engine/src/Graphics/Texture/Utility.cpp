#include "enginepch.h"
#include "Engine/Graphics/Texture/Utility.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/WebGPUUtils.h"

namespace Engine
{
	uint32_t GetMaxMipLevelCount(const wgpu::Extent3D& textureSize)
	{
		return std::bit_width(std::max(textureSize.width, textureSize.height));
	}

	void GenerateMipmaps(wgpu::Texture texture)
	{
		std::vector<wgpu::TextureView> textureMipViews;
		std::vector<wgpu::Extent3D> textureMipSizes;

		wgpu::TextureFormat format = texture.getFormat();

		wgpu::Extent3D baseSize = { texture.getWidth(), texture.getHeight(), 1 };
		auto mipCount = GetMaxMipLevelCount(baseSize);

		wgpu::TextureViewDescriptor textureViewDesc;
		textureViewDesc.nextInChain = nullptr;
		textureViewDesc.aspect = wgpu::TextureAspect::All;
		textureViewDesc.baseArrayLayer = 0;
		textureViewDesc.arrayLayerCount = 6;
		textureViewDesc.dimension = wgpu::TextureViewDimension::_2DArray;
		textureViewDesc.format = format;
		textureViewDesc.mipLevelCount = 1;


		textureMipSizes.resize(mipCount);
		textureMipViews.reserve(mipCount);

		textureMipSizes[0] = baseSize;

		for (uint32_t level = 0; level < mipCount; ++level)
		{
			std::string label = "MIP level #" + std::to_string(level);
			textureViewDesc.label = { label.c_str(), WGPU_STRLEN };
			textureViewDesc.baseMipLevel = level;
			textureMipViews.push_back(texture.createView(textureViewDesc));

			if (level > 0)
			{
				wgpu::Extent3D previousSize = textureMipSizes[level - 1];
				textureMipSizes[level] = { previousSize.width / 2, previousSize.height / 2, 6 };
			}
		}


		wgpu::ShaderModule computeShaderModule = LoadWGSLShader("Resources/Engine/shaders/compute/mipmap.wgsl");

		// Create bind group layout
		std::array<wgpu::BindGroupLayoutEntry, 2> bindings = { wgpu::Default, wgpu::Default };

		bindings[0].binding = 0;
		bindings[0].texture.sampleType = wgpu::TextureSampleType::Float;
		bindings[0].texture.viewDimension = wgpu::TextureViewDimension::_2DArray;
		bindings[0].visibility = wgpu::ShaderStage::Compute;

		bindings[1].binding = 1;
		bindings[1].storageTexture.access = wgpu::StorageTextureAccess::WriteOnly;
		bindings[1].storageTexture.format = format;
		bindings[1].storageTexture.viewDimension = wgpu::TextureViewDimension::_2DArray;
		bindings[1].visibility = wgpu::ShaderStage::Compute;

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
		computePipelineDesc.compute.entryPoint = { "computeMipMap", WGPU_STRLEN };
		computePipelineDesc.compute.module = computeShaderModule;
		computePipelineDesc.layout = pipelineLayout;
		wgpu::ComputePipeline computeMipmapPipeline = GraphicsContext::GetDevice().createComputePipeline(computePipelineDesc);


		auto CreateBindGroup = [&](uint32_t nextLevel)
			{
				std::array<wgpu::BindGroupEntry, 2> entries = { wgpu::Default, wgpu::Default };

				entries[0].binding = 0;
				entries[0].textureView = textureMipViews[nextLevel - 1];

				entries[1].binding = 1;
				entries[1].textureView = textureMipViews[nextLevel];

				wgpu::BindGroupDescriptor bindGroupDesc;
				bindGroupDesc.layout = bindGroupLayout;
				bindGroupDesc.entryCount = (uint32_t)entries.size();
				bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
				wgpu::BindGroup bindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);
				return bindGroup;
			};

		auto encoder = Engine::GraphicsContext::GetDevice().createCommandEncoder();

		wgpu::ComputePassDescriptor computePassDesc;
		computePassDesc.timestampWrites = nullptr;
		wgpu::ComputePassEncoder computePass = encoder.beginComputePass(computePassDesc);

		computePass.setPipeline(computeMipmapPipeline);


		for (uint32_t nextLevel = 1; nextLevel < mipCount; ++nextLevel)
		{
			computePass.setBindGroup(0, CreateBindGroup(nextLevel), 0, nullptr);

			uint32_t invocationCountX = textureMipSizes[nextLevel].width;
			uint32_t invocationCountY = textureMipSizes[nextLevel].height;
			uint32_t workgroupSizePerDim = 8;
			uint32_t workgroupCountX = (invocationCountX + workgroupSizePerDim - 1) / workgroupSizePerDim;
			uint32_t workgroupCountY = (invocationCountY + workgroupSizePerDim - 1) / workgroupSizePerDim;
			computePass.dispatchWorkgroups(workgroupCountX, workgroupCountY, 6);
		}

		computePass.end();

		wgpu::CommandBuffer commandBuffer = encoder.finish();
		Engine::GraphicsContext::GetQueue().submit(1, &commandBuffer);
	}

	void ImportanceSample(wgpu::Texture texture)
	{
		std::vector<wgpu::TextureView> textureMipViews;
		std::vector<wgpu::Extent3D> textureMipSizes;
		wgpu::TextureView inputTextureView;

		wgpu::TextureFormat format = texture.getFormat();

		wgpu::Extent3D baseSize = { texture.getWidth(), texture.getHeight(), 1 };
		auto mipCount = GetMaxMipLevelCount(baseSize);


		wgpu::TextureViewDescriptor textureViewDesc;
		textureViewDesc.nextInChain = nullptr;
		textureViewDesc.aspect = wgpu::TextureAspect::All;
		textureViewDesc.baseArrayLayer = 0;
		textureViewDesc.arrayLayerCount = 6;
		textureViewDesc.dimension = wgpu::TextureViewDimension::_2DArray;
		textureViewDesc.format = format;
		textureViewDesc.mipLevelCount = 1;


		textureMipSizes.resize(mipCount);
		textureMipViews.reserve(mipCount);

		textureMipSizes[0] = baseSize;

		for (uint32_t level = 0; level < mipCount; ++level)
		{
			std::string label = "MIP level #" + std::to_string(level);
			textureViewDesc.label = { label.c_str(), WGPU_STRLEN };
			textureViewDesc.baseMipLevel = level;
			textureMipViews.push_back(texture.createView(textureViewDesc));

			if (level > 0)
			{
				wgpu::Extent3D previousSize = textureMipSizes[level - 1];
				textureMipSizes[level] = { previousSize.width / 2, previousSize.height / 2, 6 };
			}
		}

		textureViewDesc.dimension = wgpu::TextureViewDimension::Cube;
		textureViewDesc.label = { "Input texture view", WGPU_STRLEN };
		textureViewDesc.baseMipLevel = 0;
		textureViewDesc.mipLevelCount = 1;
		inputTextureView = texture.createView(textureViewDesc);

		wgpu::ShaderModule computeShaderModule = LoadWGSLShader("Resources/Engine/shaders/compute/importanceSampling.wgsl");

		// Create bind group layout
		std::array<wgpu::BindGroupLayoutEntry, 3> bindings = { wgpu::Default, wgpu::Default, wgpu::Default };

		bindings[0].binding = 0;
		bindings[0].sampler.type = wgpu::SamplerBindingType::Filtering;
		bindings[0].visibility = wgpu::ShaderStage::Compute;

		bindings[1].binding = 1;
		bindings[1].texture.sampleType = wgpu::TextureSampleType::Float;
		bindings[1].texture.viewDimension = wgpu::TextureViewDimension::Cube;
		bindings[1].visibility = wgpu::ShaderStage::Compute;

		bindings[2].binding = 2;
		bindings[2].storageTexture.access = wgpu::StorageTextureAccess::WriteOnly;
		bindings[2].storageTexture.format = format;
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
		computePipelineDesc.compute.entryPoint = { "computeMipMap", WGPU_STRLEN };
		computePipelineDesc.compute.module = computeShaderModule;
		computePipelineDesc.layout = pipelineLayout;
		wgpu::ComputePipeline computeMipmapPipeline = GraphicsContext::GetDevice().createComputePipeline(computePipelineDesc);




		wgpu::SamplerDescriptor samplerDesc{};
		samplerDesc.label = { "LinearEnvSampler", WGPU_STRLEN };
		samplerDesc.magFilter = wgpu::FilterMode::Linear;
		samplerDesc.minFilter = wgpu::FilterMode::Linear;
		samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
		samplerDesc.maxAnisotropy = 1;
		wgpu::Sampler linearSampler = GraphicsContext::GetDevice().createSampler(samplerDesc);


		auto CreateBindGroup = [&](uint32_t nextLevel)
			{
				std::array<wgpu::BindGroupEntry, 3> entries = { wgpu::Default, wgpu::Default, wgpu::Default };

				entries[0].binding = 0;
				entries[0].sampler = linearSampler;

				entries[1].binding = 1;
				entries[1].textureView = inputTextureView; // the original texture is always sampled from

				entries[2].binding = 2;
				entries[2].textureView = textureMipViews[nextLevel];

				wgpu::BindGroupDescriptor bindGroupDesc;
				bindGroupDesc.layout = bindGroupLayout;
				bindGroupDesc.entryCount = (uint32_t)entries.size();
				bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
				wgpu::BindGroup bindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);
				return bindGroup;
			};

		auto encoder = Engine::GraphicsContext::GetDevice().createCommandEncoder();

		wgpu::ComputePassDescriptor computePassDesc;
		computePassDesc.timestampWrites = nullptr;
		wgpu::ComputePassEncoder computePass = encoder.beginComputePass(computePassDesc);

		computePass.setPipeline(computeMipmapPipeline);


		for (uint32_t nextLevel = 1; nextLevel < mipCount; ++nextLevel)
		{
			computePass.setBindGroup(0, CreateBindGroup(nextLevel), 0, nullptr);

			uint32_t invocationCountX = textureMipSizes[nextLevel].width;
			uint32_t invocationCountY = textureMipSizes[nextLevel].height;
			uint32_t workgroupSizePerDim = 8;
			uint32_t workgroupCountX = (invocationCountX + workgroupSizePerDim - 1) / workgroupSizePerDim;
			uint32_t workgroupCountY = (invocationCountY + workgroupSizePerDim - 1) / workgroupSizePerDim;
			computePass.dispatchWorkgroups(workgroupCountX, workgroupCountY, 6);
		}

		computePass.end();

		wgpu::CommandBuffer commandBuffer = encoder.finish();
		Engine::GraphicsContext::GetQueue().submit(1, &commandBuffer);
	}


	void ComputeIrradiance(wgpu::Texture sourceCubemap, wgpu::Texture targetCubemap)
	{
		wgpu::TextureFormat format = sourceCubemap.getFormat();

		// 1. Source View (Reading the environment map)
		wgpu::TextureViewDescriptor sourceViewDesc;
		sourceViewDesc.label = { "IrradianceSourceView", WGPU_STRLEN };
		sourceViewDesc.dimension = wgpu::TextureViewDimension::Cube;
		sourceViewDesc.format = format;
		sourceViewDesc.baseMipLevel = 0;
		sourceViewDesc.mipLevelCount = 1;
		sourceViewDesc.baseArrayLayer = 0;
		sourceViewDesc.arrayLayerCount = 6;
		sourceViewDesc.aspect = wgpu::TextureAspect::All;
		wgpu::TextureView sourceView = sourceCubemap.createView(sourceViewDesc);

		// 2. Target View (Writing directly to our pre-allocated 32x32 texture as a 2D Array)
		wgpu::TextureViewDescriptor targetViewDesc;
		targetViewDesc.label = { "IrradianceTargetView", WGPU_STRLEN };
		targetViewDesc.dimension = wgpu::TextureViewDimension::_2DArray;
		targetViewDesc.format = format;
		targetViewDesc.baseMipLevel = 0;
		targetViewDesc.mipLevelCount = 1;
		targetViewDesc.baseArrayLayer = 0;
		targetViewDesc.arrayLayerCount = 6;
		targetViewDesc.aspect = wgpu::TextureAspect::All;
		wgpu::TextureView targetView = targetCubemap.createView(targetViewDesc);

		// 3. Load Shader & Setup Layouts
		wgpu::ShaderModule computeShaderModule = LoadWGSLShader("Resources/Engine/shaders/compute/irradiance.wgsl");

		std::array<wgpu::BindGroupLayoutEntry, 3> bindings = { wgpu::Default, wgpu::Default, wgpu::Default };

		bindings[0].binding = 0;
		bindings[0].sampler.type = wgpu::SamplerBindingType::Filtering;
		bindings[0].visibility = wgpu::ShaderStage::Compute;

		bindings[1].binding = 1;
		bindings[1].texture.sampleType = wgpu::TextureSampleType::Float;
		bindings[1].texture.viewDimension = wgpu::TextureViewDimension::Cube;
		bindings[1].visibility = wgpu::ShaderStage::Compute;

		bindings[2].binding = 2;
		bindings[2].storageTexture.access = wgpu::StorageTextureAccess::WriteOnly;
		bindings[2].storageTexture.format = format;
		bindings[2].storageTexture.viewDimension = wgpu::TextureViewDimension::_2DArray;
		bindings[2].visibility = wgpu::ShaderStage::Compute;

		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
		bindGroupLayoutDesc.entryCount = bindings.size();
		bindGroupLayoutDesc.entries = bindings.data();
		wgpu::BindGroupLayout bindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(bindGroupLayoutDesc);

		wgpu::PipelineLayoutDescriptor pipelineLayoutDesc;
		pipelineLayoutDesc.bindGroupLayoutCount = 1;
		pipelineLayoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)&bindGroupLayout;
		wgpu::PipelineLayout pipelineLayout = GraphicsContext::GetDevice().createPipelineLayout(pipelineLayoutDesc);

		wgpu::ComputePipelineDescriptor computePipelineDesc = wgpu::Default;
		computePipelineDesc.compute.entryPoint = { "computeIrradiance", WGPU_STRLEN };
		computePipelineDesc.compute.module = computeShaderModule;
		computePipelineDesc.layout = pipelineLayout;
		wgpu::ComputePipeline computePipeline = GraphicsContext::GetDevice().createComputePipeline(computePipelineDesc);

		// 4. Create Sampler & Bind Group
		wgpu::SamplerDescriptor samplerDesc{};
		samplerDesc.label = { "IrradianceSampler", WGPU_STRLEN };
		samplerDesc.magFilter = wgpu::FilterMode::Linear;
		samplerDesc.minFilter = wgpu::FilterMode::Linear;
		samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
		samplerDesc.maxAnisotropy = 1;
		wgpu::Sampler linearSampler = GraphicsContext::GetDevice().createSampler(samplerDesc);

		std::array<wgpu::BindGroupEntry, 3> entries = { wgpu::Default, wgpu::Default, wgpu::Default };
		entries[0].binding = 0;
		entries[0].sampler = linearSampler;
		entries[1].binding = 1;
		entries[1].textureView = sourceView;
		entries[2].binding = 2;
		entries[2].textureView = targetView; // Bind the view pointing to the actual memory

		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.layout = bindGroupLayout;
		bindGroupDesc.entryCount = (uint32_t)entries.size();
		bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
		wgpu::BindGroup bindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);

		// 5. Encode & Dispatch
		auto encoder = Engine::GraphicsContext::GetDevice().createCommandEncoder();

		wgpu::ComputePassDescriptor computePassDesc;
		computePassDesc.timestampWrites = nullptr;
		wgpu::ComputePassEncoder computePass = encoder.beginComputePass(computePassDesc);

		computePass.setPipeline(computePipeline);
		computePass.setBindGroup(0, bindGroup, 0, nullptr);

		uint32_t irradianceSize = 32;
		uint32_t workgroupSizePerDim = 8;
		uint32_t workgroupCountX = (irradianceSize + workgroupSizePerDim - 1) / workgroupSizePerDim;
		uint32_t workgroupCountY = (irradianceSize + workgroupSizePerDim - 1) / workgroupSizePerDim;

		computePass.dispatchWorkgroups(workgroupCountX, workgroupCountY, 6);
		computePass.end();

		wgpu::CommandBuffer commandBuffer = encoder.finish();
		Engine::GraphicsContext::GetQueue().submit(1, &commandBuffer);

		// No need to return anything, the data is directly written into targetCubemap!
	}
}