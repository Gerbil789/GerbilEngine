#include "enginepch.h"
#include "Engine/Compute/MipMap.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/WebGPUUtils.h"
#include <stb_image.h>
#include "Engine/Graphics/Texture.h"
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Compute/save_texture.h"
#include "Engine/Core/Project.h"

namespace Engine
{
	namespace
	{
		wgpu::ComputePipeline m_ComputePipeline;
		wgpu::BindGroupLayout m_bindGroupLayout;
		wgpu::PipelineLayout m_pipelineLayout;

		wgpu::BindGroup m_bindGroup;


		wgpu::Texture m_texture;

		std::vector<wgpu::TextureView> m_textureMipViews;
		std::vector<wgpu::Extent3D> m_textureMipSizes;

	}


	uint32_t getMaxMipLevelCount(const wgpu::Extent3D& textureSize) 
	{
		return std::bit_width(std::max(textureSize.width, textureSize.height));
	}

	void initBindGroup(uint32_t nextLevel)
	{
		// Create compute bind group
		std::vector<wgpu::BindGroupEntry> entries(2, wgpu::Default);

		entries[0].binding = 0;
		entries[0].textureView = m_textureMipViews[nextLevel - 1];

		entries[1].binding = 1;
		entries[1].textureView = m_textureMipViews[nextLevel];

		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.layout = m_bindGroupLayout;
		bindGroupDesc.entryCount = (uint32_t)entries.size();
		bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
		m_bindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);
	}

	MipMap::MipMap()
	{
		auto path = Engine::Project::GetActive()->GetAssetsDirectory() / "Textures/input.jpg";

		//NOTE: this does not work
		TextureSpecification spec;
		spec.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::StorageBinding | wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::CopySrc;
		spec.generateMips = true;


		Texture* texture = TextureImporter::LoadTexture2D(path);

		wgpu::Extent3D baseSize = { texture->GetWidth(), texture->GetHeight(), 1 };
		auto mipCount = getMaxMipLevelCount(baseSize);
		m_texture = texture->GetTexture();

		wgpu::TextureViewDescriptor textureViewDesc;
		textureViewDesc.nextInChain = nullptr;
		textureViewDesc.aspect = wgpu::TextureAspect::All;
		textureViewDesc.baseArrayLayer = 0;
		textureViewDesc.arrayLayerCount = 1;
		textureViewDesc.dimension = wgpu::TextureViewDimension::_2D;
		textureViewDesc.format = wgpu::TextureFormat::RGBA8Unorm;
		textureViewDesc.mipLevelCount = 1;


		m_textureMipSizes.resize(mipCount);
		m_textureMipViews.reserve(mipCount);

		m_textureMipSizes[0] = baseSize;

		for (uint32_t level = 0; level < mipCount; ++level)
		{
			std::string label = "MIP level #" + std::to_string(level);
			textureViewDesc.label = { label.c_str(), WGPU_STRLEN };
			textureViewDesc.baseMipLevel = level;
			m_textureMipViews.push_back(m_texture.createView(textureViewDesc));

			if (level > 0) 
			{
				wgpu::Extent3D previousSize = m_textureMipSizes[level - 1];
				m_textureMipSizes[level] = {
						previousSize.width / 2,
						previousSize.height / 2,
						previousSize.depthOrArrayLayers / 2
				};
			}
		}

		wgpu::ShaderModule computeShaderModule = LoadWGSLShader("Resources/Engine/shaders/mipmap.wgsl");

		// Create bind group layout
		std::vector<wgpu::BindGroupLayoutEntry> bindings(2, wgpu::Default);

		bindings[0].binding = 0;
		bindings[0].texture.sampleType = wgpu::TextureSampleType::Float;
		bindings[0].texture.viewDimension = wgpu::TextureViewDimension::_2D;
		bindings[0].visibility = wgpu::ShaderStage::Compute;

		bindings[1].binding = 1;
		bindings[1].storageTexture.access = wgpu::StorageTextureAccess::WriteOnly;
		bindings[1].storageTexture.format = wgpu::TextureFormat::RGBA8Unorm;
		bindings[1].storageTexture.viewDimension = wgpu::TextureViewDimension::_2D;
		bindings[1].visibility = wgpu::ShaderStage::Compute;

		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
		bindGroupLayoutDesc.entryCount = bindings.size();
		bindGroupLayoutDesc.entries = bindings.data();
		m_bindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(bindGroupLayoutDesc);

		// Create compute pipeline layout
		wgpu::PipelineLayoutDescriptor pipelineLayoutDesc;
		pipelineLayoutDesc.bindGroupLayoutCount = 1;
		pipelineLayoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)&m_bindGroupLayout;
		m_pipelineLayout = GraphicsContext::GetDevice().createPipelineLayout(pipelineLayoutDesc);

		// Create compute pipeline;
		wgpu::ComputePipelineDescriptor computePipelineDesc = wgpu::Default;
		computePipelineDesc.compute.entryPoint = { "computeMipMap", WGPU_STRLEN };
		computePipelineDesc.compute.module = computeShaderModule;
		computePipelineDesc.layout = m_pipelineLayout;
		m_ComputePipeline = GraphicsContext::GetDevice().createComputePipeline(computePipelineDesc);






		auto encoder = Engine::GraphicsContext::GetDevice().createCommandEncoder();

		wgpu::ComputePassDescriptor computePassDesc;
		computePassDesc.timestampWrites = nullptr;
		wgpu::ComputePassEncoder computePass = encoder.beginComputePass(computePassDesc);

		computePass.setPipeline(m_ComputePipeline);

		
		for (uint32_t nextLevel = 1; nextLevel < mipCount; ++nextLevel)
		{
			initBindGroup(nextLevel);
			computePass.setBindGroup(0, m_bindGroup, 0, nullptr);

			uint32_t invocationCountX = m_textureMipSizes[nextLevel].width;
			uint32_t invocationCountY = m_textureMipSizes[nextLevel].height;
			uint32_t workgroupSizePerDim = 8;
			// This ceils invocationCountX / workgroupSizePerDim
			uint32_t workgroupCountX = (invocationCountX + workgroupSizePerDim - 1) / workgroupSizePerDim;
			uint32_t workgroupCountY = (invocationCountY + workgroupSizePerDim - 1) / workgroupSizePerDim;
			computePass.dispatchWorkgroups(workgroupCountX, workgroupCountY, 1);
		}

		computePass.end();

		wgpu::CommandBuffer commandBuffer = encoder.finish();
		Engine::GraphicsContext::GetQueue().submit(1, &commandBuffer);

		for (uint32_t nextLevel = 1; nextLevel < m_textureMipSizes.size(); ++nextLevel) 
		{
			auto outPath = Engine::Project::GetActive()->GetAssetsDirectory() / ("Textures/tmp/mip" + std::to_string(nextLevel) + ".png");
			saveTexture(outPath, m_texture, nextLevel);
		}
	}
}