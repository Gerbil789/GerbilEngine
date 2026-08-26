#include "enginepch.h"
#include "Engine/Graphics/Texture/Environment.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Texture/TextureCube.h"
#include "Engine/Graphics/Texture/Utility.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Utility.h"
#include "Engine/Asset/AssetManager.h"
#include <webgpu/webgpu_cpp.h>


namespace Engine
{
	Environment EnvironmentBaker::BakeEnvironment(Texture2D equirectangularTexture)
	{
    Environment env;
    env.EnvironmentMap = EquirectangularToCubemap(equirectangularTexture);
    env.IrradianceMap = CreateIrradianceMap(env.EnvironmentMap);
    env.PrefilteredSpecularMap = CreatePrefilteredSpecularMap(env.EnvironmentMap);
		return env;
	}

	TextureCube EnvironmentBaker::CreateIrradianceMap(const TextureCube& sourceCubemap)
	{
		wgpu::TextureFormat format = sourceCubemap.GetFormat();

    TextureCubeSpecification spec;
    spec.size = 64;
    spec.format = format;
		spec.mipCount = 1;
    TextureCube irradianceMap(spec);

		wgpu::TextureViewDescriptor targetViewDesc;
		targetViewDesc.label = "IrradianceTextureView";
		targetViewDesc.dimension = wgpu::TextureViewDimension::e2DArray;
		targetViewDesc.format = format;
		targetViewDesc.baseMipLevel = 0;
		targetViewDesc.mipLevelCount = 1;
		targetViewDesc.baseArrayLayer = 0;
		targetViewDesc.arrayLayerCount = 6;
		targetViewDesc.aspect = wgpu::TextureAspect::All;
		wgpu::TextureView targetView = irradianceMap.GetTexture().CreateView(&targetViewDesc);


		std::array<wgpu::BindGroupLayoutEntry, 3> bindings;

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
		bindings[2].storageTexture.viewDimension = wgpu::TextureViewDimension::e2DArray;
		bindings[2].visibility = wgpu::ShaderStage::Compute;

		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
		bindGroupLayoutDesc.entryCount = bindings.size();
		bindGroupLayoutDesc.entries = bindings.data();
		wgpu::BindGroupLayout bindGroupLayout = GraphicsContext::GetDevice().CreateBindGroupLayout(&bindGroupLayoutDesc);

		wgpu::PipelineLayoutDescriptor pipelineLayoutDesc;
		pipelineLayoutDesc.bindGroupLayoutCount = 1;
		pipelineLayoutDesc.bindGroupLayouts = &bindGroupLayout;
		wgpu::PipelineLayout pipelineLayout = GraphicsContext::GetDevice().CreatePipelineLayout(&pipelineLayoutDesc);

		wgpu::ShaderModule computeShaderModule = LoadWGSLShader("resources/shaders/compute/irradiance.wgsl");

		wgpu::ComputePipelineDescriptor computePipelineDesc;
		computePipelineDesc.compute.entryPoint = "computeIrradiance";
		computePipelineDesc.compute.module = computeShaderModule;
		computePipelineDesc.layout = pipelineLayout;
		wgpu::ComputePipeline computePipeline = GraphicsContext::GetDevice().CreateComputePipeline(&computePipelineDesc);

		wgpu::SamplerDescriptor samplerDesc;
		samplerDesc.label = "IrradianceSampler";
		samplerDesc.magFilter = wgpu::FilterMode::Linear;
		samplerDesc.minFilter = wgpu::FilterMode::Linear;
		samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
		samplerDesc.maxAnisotropy = 1;
		wgpu::Sampler linearSampler = GraphicsContext::GetDevice().CreateSampler(&samplerDesc);

		std::array<wgpu::BindGroupEntry, 3> entries;
		entries[0].binding = 0;
		entries[0].sampler = linearSampler;

		entries[1].binding = 1;
		entries[1].textureView = sourceCubemap.GetTextureView();

		entries[2].binding = 2;
		entries[2].textureView = targetView;

		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.layout = bindGroupLayout;
		bindGroupDesc.entryCount = entries.size();
		bindGroupDesc.entries = entries.data();
		wgpu::BindGroup bindGroup = GraphicsContext::GetDevice().CreateBindGroup(&bindGroupDesc);

		wgpu::CommandEncoder encoder = Engine::GraphicsContext::GetDevice().CreateCommandEncoder();

		wgpu::ComputePassDescriptor computePassDesc;
		computePassDesc.timestampWrites = nullptr;
		wgpu::ComputePassEncoder computePass = encoder.BeginComputePass(&computePassDesc);

		computePass.SetPipeline(computePipeline);
		computePass.SetBindGroup(0, bindGroup, 0, nullptr);

		uint32_t irradianceSize = irradianceMap.GetSize();
		uint32_t workgroupSizePerDim = 8;
		uint32_t workgroupCountX = (irradianceSize + workgroupSizePerDim - 1) / workgroupSizePerDim;
		uint32_t workgroupCountY = (irradianceSize + workgroupSizePerDim - 1) / workgroupSizePerDim;

		computePass.DispatchWorkgroups(workgroupCountX, workgroupCountY, 6);
		computePass.End();

		wgpu::CommandBuffer commandBuffer = encoder.Finish();
		Engine::GraphicsContext::GetQueue().Submit(1, &commandBuffer);

    return irradianceMap;
	}

	TextureCube EnvironmentBaker::CreatePrefilteredSpecularMap(const TextureCube& sourceCubemap)
	{
		constexpr double PREFILTER_SIZE = 256.0; // size of the prefiltered specular map

		wgpu::TextureFormat format = sourceCubemap.GetFormat();
		wgpu::Extent3D baseSize = { static_cast<uint32_t>(PREFILTER_SIZE), static_cast<uint32_t>(PREFILTER_SIZE), 6 };
		uint32_t mipCount = GetMaxMipLevelCount(baseSize);

    TextureCubeSpecification spec;
    spec.size = static_cast<uint32_t>(PREFILTER_SIZE);
    spec.format = format;
    spec.mipCount = mipCount;
    TextureCube prefilteredSpecularMap(spec);

		std::vector<wgpu::TextureView> textureMipViews;
		std::vector<wgpu::Extent3D> textureMipSizes;
		textureMipSizes.resize(mipCount);
		textureMipViews.reserve(mipCount);

		// Create texture views for each mip level of the prefiltered specular map
		{
			wgpu::TextureViewDescriptor textureViewDesc;
			textureViewDesc.nextInChain = nullptr;
			textureViewDesc.aspect = wgpu::TextureAspect::All;
			textureViewDesc.baseArrayLayer = 0;
			textureViewDesc.arrayLayerCount = 6;
			textureViewDesc.dimension = wgpu::TextureViewDimension::e2DArray;
			textureViewDesc.format = format;
			textureViewDesc.mipLevelCount = 1;

			textureMipSizes[0] = baseSize;

			for (uint32_t level = 0; level < mipCount; ++level)
			{
				std::string label = std::format("PrefilteredSpecularMipLevel#{}", level);
				textureViewDesc.label = label.c_str();
				textureViewDesc.baseMipLevel = level;
				textureMipViews.push_back(prefilteredSpecularMap.GetTexture().CreateView(&textureViewDesc));

				if (level > 0)
				{
					wgpu::Extent3D previousSize = textureMipSizes[level - 1];
					textureMipSizes[level] = { previousSize.width / 2, previousSize.height / 2, 6 };
				}
			}
		}
		
		std::array<wgpu::BindGroupLayoutEntry, 3> bindings;

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
		bindings[2].storageTexture.viewDimension = wgpu::TextureViewDimension::e2DArray;
		bindings[2].visibility = wgpu::ShaderStage::Compute;

		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
		bindGroupLayoutDesc.entryCount = bindings.size();
		bindGroupLayoutDesc.entries = bindings.data();
		wgpu::BindGroupLayout bindGroupLayout = GraphicsContext::GetDevice().CreateBindGroupLayout(&bindGroupLayoutDesc);

		wgpu::PipelineLayoutDescriptor pipelineLayoutDesc;
		pipelineLayoutDesc.bindGroupLayoutCount = 1;
		pipelineLayoutDesc.bindGroupLayouts = &bindGroupLayout;
		wgpu::PipelineLayout pipelineLayout = GraphicsContext::GetDevice().CreatePipelineLayout(&pipelineLayoutDesc);

		wgpu::ShaderModule computeShaderModule = LoadWGSLShader("resources/shaders/compute/importanceSampling.wgsl");

		wgpu::ConstantEntry overrideConstants[1];
		overrideConstants[0].key = "PREFILTER_SIZE";
		overrideConstants[0].value = PREFILTER_SIZE;

		wgpu::ComputePipelineDescriptor computePipelineDesc;
		computePipelineDesc.compute.entryPoint = "computeMipMap";
		computePipelineDesc.compute.module = computeShaderModule;
		computePipelineDesc.compute.constantCount = 1;
		computePipelineDesc.compute.constants = overrideConstants;
		computePipelineDesc.layout = pipelineLayout;
		wgpu::ComputePipeline computeMipmapPipeline = GraphicsContext::GetDevice().CreateComputePipeline(&computePipelineDesc);

		wgpu::SamplerDescriptor samplerDesc;
		samplerDesc.label = "PrefilteredSpecularSampler";
		samplerDesc.magFilter = wgpu::FilterMode::Linear;
		samplerDesc.minFilter = wgpu::FilterMode::Linear;
		samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
		samplerDesc.maxAnisotropy = 1;
		samplerDesc.lodMinClamp = 0.0f;
		samplerDesc.lodMaxClamp = static_cast<float>(mipCount - 1);
		wgpu::Sampler linearSampler = GraphicsContext::GetDevice().CreateSampler(&samplerDesc);

		auto CreateBindGroup = [&](uint32_t nextLevel)
			{
				std::array<wgpu::BindGroupEntry, 3> entries{};

				entries[0].binding = 0;
				entries[0].sampler = linearSampler;

				entries[1].binding = 1;
				entries[1].textureView = sourceCubemap.GetTextureView();

				entries[2].binding = 2;
				entries[2].textureView = textureMipViews[nextLevel];

				wgpu::BindGroupDescriptor bindGroupDesc;
				bindGroupDesc.layout = bindGroupLayout;
				bindGroupDesc.entryCount = entries.size();
				bindGroupDesc.entries = entries.data();
				wgpu::BindGroup bindGroup = GraphicsContext::GetDevice().CreateBindGroup(&bindGroupDesc);
				return bindGroup;
			};

		auto encoder = Engine::GraphicsContext::GetDevice().CreateCommandEncoder();

		wgpu::ComputePassDescriptor computePassDesc;
		computePassDesc.timestampWrites = nullptr;
		wgpu::ComputePassEncoder computePass = encoder.BeginComputePass(&computePassDesc);

		computePass.SetPipeline(computeMipmapPipeline);

		for (uint32_t nextLevel = 0; nextLevel < mipCount; ++nextLevel)
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

    return prefilteredSpecularMap;
	}
}