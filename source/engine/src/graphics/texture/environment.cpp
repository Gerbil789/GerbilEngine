#include "enginepch.h"
#include "engine/graphics/texture/environment.h"
#include "engine/graphics/texture/texture_2d.h"
#include "engine/graphics/texture/utility.h"
#include "engine/graphics/graphics_context.h"
#include "engine/graphics/utility.h"
#include <webgpu/webgpu_cpp.h>

namespace engine
{
	Environment EnvironmentBaker::BakeEnvironment(TextureHandle equirectangulartexture)
	{
    Environment env;
    env.EnvironmentMap = EquirectangularToCubemap(equirectangulartexture);
    env.IrradianceMap = CreateIrradianceMap(env.EnvironmentMap);
    env.PrefilteredSpecularMap = CreatePrefilteredSpecularMap(env.EnvironmentMap);
		return env;
	}

	textureCube EnvironmentBaker::CreateIrradianceMap(const textureCube& sourceCubemap)
	{
		wgpu::TextureFormat format = sourceCubemap.GetFormat();

    textureCubeSpecification spec;
    spec.size = 64;
    spec.format = format;
		spec.mipCount = 1;
    textureCube irradianceMap(spec);

		wgpu::TextureViewDescriptor targetViewDesc;
		targetViewDesc.label = "IrradiancetextureView";
		targetViewDesc.dimension = wgpu::TextureViewDimension::e2DArray;
		targetViewDesc.format = format;
		targetViewDesc.baseMipLevel = 0;
		targetViewDesc.mipLevelCount = 1;
		targetViewDesc.baseArrayLayer = 0;
		targetViewDesc.arrayLayerCount = 6;
		targetViewDesc.aspect = wgpu::TextureAspect::All;
		wgpu::TextureView targetView = irradianceMap.Gettexture().CreateView(&targetViewDesc);


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
		wgpu::BindGroupLayout bindGroupLayout = graphicsContext::GetDevice().CreateBindGroupLayout(&bindGroupLayoutDesc);

		wgpu::PipelineLayoutDescriptor pipelineLayoutDesc;
		pipelineLayoutDesc.bindGroupLayoutCount = 1;
		pipelineLayoutDesc.bindGroupLayouts = &bindGroupLayout;
		wgpu::PipelineLayout pipelineLayout = graphicsContext::GetDevice().CreatePipelineLayout(&pipelineLayoutDesc);

		wgpu::ShaderModule computeShaderModule = LoadWGSLShader("resources/shaders/compute/irradiance.wgsl");

		wgpu::ComputePipelineDescriptor computePipelineDesc;
		computePipelineDesc.compute.entryPoint = "computeIrradiance";
		computePipelineDesc.compute.module = computeShaderModule;
		computePipelineDesc.layout = pipelineLayout;
		wgpu::ComputePipeline computePipeline = graphicsContext::GetDevice().CreateComputePipeline(&computePipelineDesc);

		wgpu::SamplerDescriptor samplerDesc;
		samplerDesc.label = "IrradianceSampler";
		samplerDesc.magFilter = wgpu::FilterMode::Linear;
		samplerDesc.minFilter = wgpu::FilterMode::Linear;
		samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
		samplerDesc.maxAnisotropy = 1;
		wgpu::Sampler linearSampler = graphicsContext::GetDevice().CreateSampler(&samplerDesc);

		std::array<wgpu::BindGroupEntry, 3> entries;
		entries[0].binding = 0;
		entries[0].sampler = linearSampler;

		entries[1].binding = 1;
		entries[1].textureView = sourceCubemap.GettextureView();

		entries[2].binding = 2;
		entries[2].textureView = targetView;

		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.layout = bindGroupLayout;
		bindGroupDesc.entryCount = entries.size();
		bindGroupDesc.entries = entries.data();
		wgpu::BindGroup bindGroup = graphicsContext::GetDevice().CreateBindGroup(&bindGroupDesc);

		wgpu::CommandEncoder encoder = engine::graphicsContext::GetDevice().CreateCommandEncoder();

		wgpu::ComputePassDescriptor computepassDesc;
		computepassDesc.timestampWrites = nullptr;
		wgpu::ComputePassEncoder computepass = encoder.BeginComputePass(&computepassDesc);

		computepass.SetPipeline(computePipeline);
		computepass.SetBindGroup(0, bindGroup, 0, nullptr);

		uint32_t irradianceSize = irradianceMap.GetSize();
		uint32_t workgroupSizePerDim = 8;
		uint32_t workgroupCountX = (irradianceSize + workgroupSizePerDim - 1) / workgroupSizePerDim;
		uint32_t workgroupCountY = (irradianceSize + workgroupSizePerDim - 1) / workgroupSizePerDim;

		computepass.DispatchWorkgroups(workgroupCountX, workgroupCountY, 6);
		computepass.End();

		wgpu::CommandBuffer commandBuffer = encoder.Finish();
		engine::graphicsContext::GetQueue().Submit(1, &commandBuffer);

    return irradianceMap;
	}

	textureCube EnvironmentBaker::CreatePrefilteredSpecularMap(const textureCube& sourceCubemap)
	{
		constexpr double PREFILTER_SIZE = 256.0; // size of the prefiltered specular map

		wgpu::TextureFormat format = sourceCubemap.GetFormat();
		wgpu::Extent3D baseSize = { static_cast<uint32_t>(PREFILTER_SIZE), static_cast<uint32_t>(PREFILTER_SIZE), 6 };
		uint32_t mipCount = GetMaxMipLevelCount(baseSize.width, baseSize.height);

    textureCubeSpecification spec;
    spec.size = static_cast<uint32_t>(PREFILTER_SIZE);
    spec.format = format;
    spec.mipCount = mipCount;
    textureCube prefilteredSpecularMap(spec);

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
				textureMipViews.push_back(prefilteredSpecularMap.Gettexture().CreateView(&textureViewDesc));

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
		wgpu::BindGroupLayout bindGroupLayout = graphicsContext::GetDevice().CreateBindGroupLayout(&bindGroupLayoutDesc);

		wgpu::PipelineLayoutDescriptor pipelineLayoutDesc;
		pipelineLayoutDesc.bindGroupLayoutCount = 1;
		pipelineLayoutDesc.bindGroupLayouts = &bindGroupLayout;
		wgpu::PipelineLayout pipelineLayout = graphicsContext::GetDevice().CreatePipelineLayout(&pipelineLayoutDesc);

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
		wgpu::ComputePipeline computeMipmapPipeline = graphicsContext::GetDevice().CreateComputePipeline(&computePipelineDesc);

		wgpu::SamplerDescriptor samplerDesc;
		samplerDesc.label = "PrefilteredSpecularSampler";
		samplerDesc.magFilter = wgpu::FilterMode::Linear;
		samplerDesc.minFilter = wgpu::FilterMode::Linear;
		samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
		samplerDesc.maxAnisotropy = 1;
		samplerDesc.lodMinClamp = 0.0f;
		samplerDesc.lodMaxClamp = static_cast<float>(mipCount - 1);
		wgpu::Sampler linearSampler = graphicsContext::GetDevice().CreateSampler(&samplerDesc);

		auto CreateBindGroup = [&linearSampler, &sourceCubemap, &textureMipViews, &bindGroupLayout](uint32_t nextLevel)
			{
				std::array<wgpu::BindGroupEntry, 3> entries{};

				entries[0].binding = 0;
				entries[0].sampler = linearSampler;

				entries[1].binding = 1;
				entries[1].textureView = sourceCubemap.GettextureView();

				entries[2].binding = 2;
				entries[2].textureView = textureMipViews[nextLevel];

				wgpu::BindGroupDescriptor bindGroupDesc;
				bindGroupDesc.layout = bindGroupLayout;
				bindGroupDesc.entryCount = entries.size();
				bindGroupDesc.entries = entries.data();
				wgpu::BindGroup bindGroup = graphicsContext::GetDevice().CreateBindGroup(&bindGroupDesc);
				return bindGroup;
			};

		auto encoder = engine::graphicsContext::GetDevice().CreateCommandEncoder();

		wgpu::ComputePassDescriptor computepassDesc;
		computepassDesc.timestampWrites = nullptr;
		wgpu::ComputePassEncoder computepass = encoder.BeginComputePass(&computepassDesc);

		computepass.SetPipeline(computeMipmapPipeline);

		for (uint32_t nextLevel = 0; nextLevel < mipCount; ++nextLevel)
		{
			computepass.SetBindGroup(0, CreateBindGroup(nextLevel), 0, nullptr);

			uint32_t invocationCountX = textureMipSizes[nextLevel].width;
			uint32_t invocationCountY = textureMipSizes[nextLevel].height;
			uint32_t workgroupSizePerDim = 8;
			uint32_t workgroupCountX = (invocationCountX + workgroupSizePerDim - 1) / workgroupSizePerDim;
			uint32_t workgroupCountY = (invocationCountY + workgroupSizePerDim - 1) / workgroupSizePerDim;
			computepass.DispatchWorkgroups(workgroupCountX, workgroupCountY, 6);
		}

		computepass.End();

		wgpu::CommandBuffer commandBuffer = encoder.Finish();
		engine::graphicsContext::GetQueue().Submit(1, &commandBuffer);

    return prefilteredSpecularMap;
	}
}