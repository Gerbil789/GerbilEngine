#include "enginepch.h"
#include "Engine/Graphics/Texture/Environment.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Texture/TextureCube.h"
#include "Engine/Graphics/Texture/Utility.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/WebGPUUtils.h"
#include <webgpu/webgpu-raii.hpp>

namespace Engine
{
	Environment EnvironmentBaker::BakeEnvironment(Texture2D& equirectangularHDR)
	{
    Environment env;
		env.TextureHDR = equirectangularHDR;
		env.BaseCubemap = EquirectangularToCubemap(equirectangularHDR);
    env.IrradianceMap = CalculateIrradiance(env.BaseCubemap);
    env.PrefilteredMap = CalculatePrefiltered(env.BaseCubemap);
		return env;
	}

  TextureCube EnvironmentBaker::EquirectangularToCubemap(Texture2D& source)
  {
    uint32_t faceSize = source.GetHeight() / 2;
    wgpu::TextureFormat format = source.GetFormat();

    TextureSpecification spec;
    spec.width = faceSize;
    spec.height = faceSize;
    spec.format = format;
    spec.usage = wgpu::TextureUsage::StorageBinding | wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::CopySrc;
    spec.generateMips = true;

    TextureCube targetCubemap(spec);

    // View for READING the 2D Equirectangular source
    wgpu::TextureViewDescriptor sourceViewDesc;
    sourceViewDesc.format = format;
    sourceViewDesc.dimension = wgpu::TextureViewDimension::_2D;
    sourceViewDesc.baseMipLevel = 0;
    sourceViewDesc.mipLevelCount = 1;
    sourceViewDesc.baseArrayLayer = 0;
    sourceViewDesc.arrayLayerCount = 1;
    sourceViewDesc.aspect = wgpu::TextureAspect::All;
    wgpu::TextureView sourceTextureView = source.GetTexture().createView(sourceViewDesc);

    // View for WRITING to the 6 faces of the target cubemap (Mip Level 0)
    wgpu::TextureViewDescriptor targetViewDesc;
    targetViewDesc.format = format;
    targetViewDesc.dimension = wgpu::TextureViewDimension::_2DArray;
    targetViewDesc.baseMipLevel = 0;      // We only write to the top mip level
    targetViewDesc.mipLevelCount = 1;
    targetViewDesc.baseArrayLayer = 0;
    targetViewDesc.arrayLayerCount = 6;   // Write to all 6 faces
    targetViewDesc.aspect = wgpu::TextureAspect::All;
    wgpu::TextureView targetWriteView = targetCubemap.GetTexture().createView(targetViewDesc);


    // (Note: In a real engine, you should cache 'computeShaderModule' and 'computePipeline' 
    // so you don't recreate them every time you load a new skybox!)
    wgpu::ShaderModule computeShaderModule = LoadWGSLShader("Resources/Engine/shaders/compute/cubemap.wgsl");

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
    computePipelineDesc.compute.entryPoint = { "equirectToCubemap", WGPU_STRLEN };
    computePipelineDesc.compute.module = computeShaderModule;
    computePipelineDesc.layout = pipelineLayout;
    wgpu::ComputePipeline computePipeline = GraphicsContext::GetDevice().createComputePipeline(computePipelineDesc);

    wgpu::SamplerDescriptor samplerDesc;
    samplerDesc.magFilter = wgpu::FilterMode::Linear;
    samplerDesc.minFilter = wgpu::FilterMode::Linear;
		samplerDesc.maxAnisotropy = 1;
    wgpu::Sampler linearSampler = GraphicsContext::GetDevice().createSampler(samplerDesc);

    std::vector<wgpu::BindGroupEntry> entries(3, wgpu::Default);
    entries[0].binding = 0;
    entries[0].sampler = linearSampler;
    entries[1].binding = 1;
    entries[1].textureView = sourceTextureView;
    entries[2].binding = 2;
    entries[2].textureView = targetWriteView; // Write to our new cubemap array view

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

    uint32_t workgroupSizePerDim = 4; // Matches your WGSL @workgroup_size(4, 4, 1)
    uint32_t workgroupCountX = (faceSize + workgroupSizePerDim - 1) / workgroupSizePerDim;
    uint32_t workgroupCountY = (faceSize + workgroupSizePerDim - 1) / workgroupSizePerDim;

    computePass.dispatchWorkgroups(workgroupCountX, workgroupCountY, 6);
    computePass.end();

    wgpu::CommandBuffer commandBuffer = encoder.finish();
    Engine::GraphicsContext::GetQueue().submit(1, &commandBuffer);

    return targetCubemap;
  }

	TextureCube EnvironmentBaker::CalculateIrradiance(TextureCube& sourceCubemap)
	{
    TextureSpecification spec;
    spec.width = 64;
    spec.height = 64;
    spec.format = sourceCubemap.GetFormat();
    spec.usage = wgpu::TextureUsage::StorageBinding | wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopySrc;
    spec.generateMips = false;

    TextureCube irradianceMap(spec);

    ComputeIrradiance(sourceCubemap.GetTexture(), irradianceMap.GetTexture());

    return irradianceMap;
	}

	TextureCube EnvironmentBaker::CalculatePrefiltered(TextureCube& sourceCubemap)
	{
		ImportanceSample(sourceCubemap.GetTexture());
		return sourceCubemap;
		
	}
}