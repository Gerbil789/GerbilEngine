#include "enginepch.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Core/Scene.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Texture/TextureCube.h"
#include "Engine/Graphics/Renderer/DrawList.h"
#include "Engine/Core/Components.h"
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Graphics/Renderer/RenderUniforms.h"
#include "Engine/Graphics/RenderPass/RenderPassRegistry.h"
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"
#include "Engine/Graphics/Texture/Environment.h"
#include "Engine/Asset/Resources.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/System/CameraSystem.h"
#include <glm/gtx/quaternion.hpp>

namespace engine
{
	void Renderer::Initialize()
	{
		CreateViewUniformBuffer();
		CreateViewBindGroup();

		CreateModelStorageBuffer();
		CreateModelBindGroup();

		CreateShadowTexture();

		CreateEnvironmentUniformBuffer();
		SetEnvironmentTexture(RESOURCES::TEXTURE::HDR);
	}

	void Renderer::SetColorTarget(wgpu::TextureView colorView)
	{
		m_RenderContext.colorTarget = colorView;
	}

	void Renderer::SetSize(float width, float height)
	{
		m_RenderContext.width = width;
		m_RenderContext.height = height;

		wgpu::TextureDescriptor depthDesc;
		depthDesc.label = "RendererDepthTexture";
		depthDesc.dimension = wgpu::TextureDimension::e2D;
		depthDesc.format = wgpu::TextureFormat::Depth24Plus;
		depthDesc.mipLevelCount = 1;
		depthDesc.sampleCount = 1;
		depthDesc.size = {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1};
		depthDesc.usage = wgpu::TextureUsage::RenderAttachment;

		m_DepthTexture = engine::GraphicsContext::GetDevice().CreateTexture(&depthDesc);

		wgpu::TextureViewDescriptor depthViewDesc;
		depthViewDesc.label = "RendererDepthTextureView";
		depthViewDesc.aspect = wgpu::TextureAspect::DepthOnly;
		depthViewDesc.dimension = wgpu::TextureViewDimension::e2D;
		depthViewDesc.format = wgpu::TextureFormat::Depth24Plus;
		depthViewDesc.baseMipLevel = 0;
		depthViewDesc.mipLevelCount = 1;
		depthViewDesc.baseArrayLayer = 0;
		depthViewDesc.arrayLayerCount = 1;

		m_RenderContext.depthTarget = m_DepthTexture.CreateView(&depthViewDesc);
	}

	void Renderer::SetEnvironmentTexture(Texture2D texture)
	{
		if (!texture)
		{
			texture = RESOURCES::TEXTURE::HDR;
		}

		m_RenderContext.environment = EnvironmentBaker::BakeEnvironment(texture);
		CreateEnvironmentBindGroup();
	}

	void Renderer::CreateViewUniformBuffer()
	{
		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.label = "ViewUniformBuffer";
		bufferDesc.size = sizeof(ViewUniforms);
		bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
		m_RenderContext.viewUniformBuffer = GraphicsContext::GetDevice().CreateBuffer(&bufferDesc);
	}

	void Renderer::CreateViewBindGroup()
	{
		std::array<wgpu::BindGroupEntry, 1> entries;

		entries[0].binding = 0;
		entries[0].buffer = m_RenderContext.viewUniformBuffer;
		entries[0].offset = 0;
		entries[0].size = sizeof(ViewUniforms);

		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.label = "ViewBindGroup";
		bindGroupDesc.layout = RenderPipelineLayouts::GetViewLayout();
		bindGroupDesc.entryCount = entries.size();
		bindGroupDesc.entries = entries.data();
		m_RenderContext.viewBindGroup = GraphicsContext::GetDevice().CreateBindGroup(&bindGroupDesc);
	}

	void Renderer::CreateModelStorageBuffer()
	{
		// max 1024 * 256 unique transforms per frame //TODO: make this configurable or dynamic
		const uint64_t bufferSize = 1024 * 256 * sizeof(glm::mat4);

		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.label = "ModelStorageBuffer";
		bufferDesc.size = bufferSize;
		bufferDesc.usage = wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst;
		m_RenderContext.modelStorageBuffer = GraphicsContext::GetDevice().CreateBuffer(&bufferDesc);
	}

	void Renderer::CreateModelBindGroup()
	{
		wgpu::BindGroupEntry bindGroupEntry;
		bindGroupEntry.binding = 0;
		bindGroupEntry.buffer = m_RenderContext.modelStorageBuffer;
		bindGroupEntry.offset = 0;
		bindGroupEntry.size = m_RenderContext.modelStorageBuffer.GetSize();

		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.label = "ModelBindGroup";
		bindGroupDesc.layout = RenderPipelineLayouts::GetModelLayout();
		bindGroupDesc.entryCount = 1;
		bindGroupDesc.entries = &bindGroupEntry;
		m_RenderContext.modelBindGroup = GraphicsContext::GetDevice().CreateBindGroup(&bindGroupDesc);
	}

	void Renderer::CreateEnvironmentUniformBuffer()
	{
		wgpu::BufferDescriptor desc;
		desc.label = "EnvironmentUniformBuffer";
		desc.size = sizeof(EnvironmentUniforms);
		desc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
		m_RenderContext.environmentUniformBuffer = GraphicsContext::GetDevice().CreateBuffer(&desc);
	}

	void Renderer::CreateEnvironmentBindGroup()
	{
		std::array<wgpu::BindGroupEntry, 8> entries;

		// 0 - EnvironmentSampler
		{
			wgpu::SamplerDescriptor envSamplerDesc;
			envSamplerDesc.label = "EnvironmentSampler";
			envSamplerDesc.minFilter = wgpu::FilterMode::Linear;
			envSamplerDesc.magFilter = wgpu::FilterMode::Linear;
			envSamplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
			envSamplerDesc.maxAnisotropy = 1;
			envSamplerDesc.lodMinClamp = 0.0f;
			envSamplerDesc.lodMaxClamp = 32.0f;

			wgpu::Sampler envSampler = GraphicsContext::GetDevice().CreateSampler(&envSamplerDesc);
			entries[0].binding = 0;
			entries[0].sampler = envSampler;
		}

		// 1 - EnvironmentMap
		{
			entries[1].binding = 1;
			entries[1].textureView = m_RenderContext.environment.EnvironmentMap.GetTextureView();
		}

		// 2 - IrradianceMap
		{
			entries[2].binding = 2;
			entries[2].textureView = m_RenderContext.environment.IrradianceMap.GetTextureView();
		}

		// 3 - PrefilteredSpecularMap
		{
			entries[3].binding = 3;
			entries[3].textureView = m_RenderContext.environment.PrefilteredSpecularMap.GetTextureView();
		}

		// 4 - BRDFIntMap
		{
			auto brdfTexture = TextureImporter::LoadTexture2D("resources/hdr/brdf_integration_map_ct_ggx.hdr").value(); // TODO: is this memory leak?
			entries[4].binding = 4;
			entries[4].textureView = brdfTexture.GetTextureView();
		}

		// 5 - ShadowUniforms
		{
			entries[5].binding = 5;
			entries[5].buffer = m_RenderContext.environmentUniformBuffer;
			entries[5].offset = 0;
			entries[5].size = sizeof(EnvironmentUniforms);
		}

		// 6 -ShadowSampler
		{
			wgpu::SamplerDescriptor desc;
			desc.label = "ShadowSampler";
			desc.compare = wgpu::CompareFunction::LessEqual;
			desc.minFilter = wgpu::FilterMode::Linear;
			desc.magFilter = wgpu::FilterMode::Linear;
			desc.maxAnisotropy = 1;
			wgpu::Sampler shadowSampler = GraphicsContext::GetDevice().CreateSampler(&desc);

			entries[6].binding = 6;
			entries[6].sampler = shadowSampler;
		}

		// 7 - ShadowMap
		{
			entries[7].binding = 7;
			entries[7].textureView = m_RenderContext.depthTextureArrayView;
		}

		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.label = "EnvironmentBindGroup";
		bindGroupDesc.layout = RenderPipelineLayouts::GetEnvironmentLayout();
		bindGroupDesc.entryCount = entries.size();
		bindGroupDesc.entries = entries.data();
		m_RenderContext.environmentBindGroup = GraphicsContext::GetDevice().CreateBindGroup(&bindGroupDesc);
	}

	void Renderer::CreateShadowTexture()
	{
		wgpu::TextureFormat format = wgpu::TextureFormat::Depth24Plus;

		wgpu::TextureDescriptor textureDesc;
		textureDesc.dimension = wgpu::TextureDimension::e2D;
		textureDesc.format = format;
		textureDesc.mipLevelCount = 1;
		textureDesc.sampleCount = 1;
		textureDesc.size = {1024, 1024, s_ShadowCascadeCount};
		textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc;
		textureDesc.viewFormatCount = 0;
		textureDesc.viewFormats = nullptr;
		wgpu::Texture texture = GraphicsContext::GetDevice().CreateTexture(&textureDesc);

		for (int i = 0; i < s_ShadowCascadeCount; i++)
		{
			wgpu::TextureViewDescriptor viewDesc;
			viewDesc.format = format;
			viewDesc.aspect = wgpu::TextureAspect::DepthOnly;
			viewDesc.dimension = wgpu::TextureViewDimension::e2D;
			viewDesc.baseMipLevel = 0;
			viewDesc.mipLevelCount = 1;
			viewDesc.baseArrayLayer = i;
			viewDesc.arrayLayerCount = 1;

			m_RenderContext.depthTextureViews[i] = texture.CreateView(&viewDesc);
		}

		wgpu::TextureViewDescriptor arrayViewDesc;
		arrayViewDesc.format = format;
		arrayViewDesc.aspect = wgpu::TextureAspect::DepthOnly;
		arrayViewDesc.dimension = wgpu::TextureViewDimension::e2DArray;
		arrayViewDesc.baseMipLevel = 0;
		arrayViewDesc.mipLevelCount = 1;
		arrayViewDesc.baseArrayLayer = 0;
		arrayViewDesc.arrayLayerCount = s_ShadowCascadeCount;

		m_RenderContext.depthTextureArrayView = texture.CreateView(&arrayViewDesc);
	}

	void Renderer::RenderScene(SceneAsset &scene)
	{
		m_RenderContext.scene = &scene;

		CameraSystem::Update(scene.GetRegistry(), m_RenderContext.width / m_RenderContext.height);

		ViewUniforms viewUniforms;
		viewUniforms.view = m_RenderContext.camera->viewMatrix;
		viewUniforms.projection = m_RenderContext.camera->projectionMatrix;
		viewUniforms.cameraPosition = m_RenderContext.cameraTransform->position;

		GraphicsContext::GetQueue().WriteBuffer(m_RenderContext.viewUniformBuffer, 0, &viewUniforms, sizeof(viewUniforms));

		wgpu::CommandEncoder encoder = GraphicsContext::GetDevice().CreateCommandEncoder();

		m_RenderContext.drawList = DrawList::CreateFromScene(scene);

		const std::vector<glm::mat4> &modelMatrices = m_RenderContext.drawList.GetTransforms();

		GraphicsContext::GetQueue().WriteBuffer(m_RenderContext.modelStorageBuffer, 0, modelMatrices.data(), modelMatrices.size() * sizeof(glm::mat4));

		static const RenderPassType order[] = {
				RenderPassType::Shadow,
				RenderPassType::Background,
				RenderPassType::Opaque,
				// RenderPassType::Light,
				RenderPassType::Normal,
				RenderPassType::Wireframe,
				RenderPassType::UI};

		for (RenderPassType type : order)
		{
			if ((m_EnabledPasses & type) != RenderPassType::None)
			{
				auto *pass = RenderPassRegistry::GetPass(type);
				if (pass)
				{
					pass->Execute(encoder, m_RenderContext);
				}
				else
				{
					LOG_ERROR("Render pass not found for type: {}", static_cast<uint32_t>(type)); // TODO: C++26 reflection here
				}
			}
		}

		wgpu::CommandBuffer commandBuffer = encoder.Finish();
		GraphicsContext::GetQueue().Submit(1, &commandBuffer);
	}

	wgpu::TextureView Renderer::GetTextureView() const
	{
		return m_RenderContext.colorTarget;
	}
}