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
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"
#include "Engine/Graphics/Texture/Environment.h"
#include "Engine/Asset/Resources.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/System/CameraSystem.h"
#include <glm/gtx/quaternion.hpp>

namespace engine
{
	wgpu::Buffer CreateViewUniformBuffer()
	{
		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.label = "ViewUniformBuffer";
		bufferDesc.size = sizeof(ViewUniforms);
		bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
		return GraphicsContext::GetDevice().CreateBuffer(&bufferDesc);
	}

	wgpu::BindGroup CreateViewBindGroup(wgpu::Buffer buffer)
	{
		wgpu::BindGroupEntry entry;
		entry.binding = 0;
		entry.buffer = buffer;
		entry.offset = 0;
		entry.size = sizeof(ViewUniforms);

		wgpu::BindGroupDescriptor desc;
		desc.label = "ViewBindGroup";
		desc.layout = RenderPipelineLayouts::GetViewLayout();
		desc.entryCount = 1;
		desc.entries = &entry;
		return GraphicsContext::GetDevice().CreateBindGroup(&desc);
	}

	wgpu::Buffer CreateModelStorageBuffer()
	{
		// max 1024 * 256 unique transforms per frame //TODO: make this configurable or dynamic
		const uint64_t bufferSize = 1024 * 256 * sizeof(glm::mat4);

		wgpu::BufferDescriptor desc;
		desc.label = "ModelStorageBuffer";
		desc.size = bufferSize;
		desc.usage = wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst;
		return GraphicsContext::GetDevice().CreateBuffer(&desc);
	}

	wgpu::BindGroup CreateModelBindGroup(wgpu::Buffer buffer)
	{
		wgpu::BindGroupEntry entry;
		entry.binding = 0;
		entry.buffer = buffer;
		entry.offset = 0;
		entry.size = buffer.GetSize();

		wgpu::BindGroupDescriptor desc;
		desc.label = "ModelBindGroup";
		desc.layout = RenderPipelineLayouts::GetModelLayout();
		desc.entryCount = 1;
		desc.entries = &entry;
		return GraphicsContext::GetDevice().CreateBindGroup(&desc);
	}


	void Renderer::Initialize(uint32_t width, uint32_t height, std::vector<RenderPass> passes)
	{
		m_ViewUniformBuffer = CreateViewUniformBuffer();
		m_FrameContext.viewBindGroup = CreateViewBindGroup(m_ViewUniformBuffer);

		m_ModelStorageBuffer = CreateModelStorageBuffer();
		m_FrameContext.modelBindGroup = CreateModelBindGroup(m_ModelStorageBuffer);

		CreateShadowTexture();

		CreateEnvironmentUniformBuffer();
		SetEnvironment(RESOURCES::TEXTURE::HDR);

		SetSize(width, height);

		m_Passes = std::move(passes);
	}


	void Renderer::SetSize(uint32_t width, uint32_t height)
	{
		// depth
		{
			wgpu::TextureDescriptor desc;
			desc.label = "RendererDepthTexture";
			desc.dimension = wgpu::TextureDimension::e2D;
			desc.format = wgpu::TextureFormat::Depth24Plus;
			desc.mipLevelCount = 1;
			desc.sampleCount = 1;
			desc.size = { width, height, 1 };
			desc.usage = wgpu::TextureUsage::RenderAttachment;
			m_DepthTexture = engine::GraphicsContext::GetDevice().CreateTexture(&desc);
		}

		{
			wgpu::TextureViewDescriptor desc;
			desc.label = "RendererDepthTextureView";
			desc.aspect = wgpu::TextureAspect::DepthOnly;
			desc.dimension = wgpu::TextureViewDimension::e2D;
			desc.format = m_DepthTexture.GetFormat();
			desc.baseMipLevel = 0;
			desc.mipLevelCount = 1;
			desc.baseArrayLayer = 0;
			desc.arrayLayerCount = 1;
			m_FrameContext.depthTarget = m_DepthTexture.CreateView(&desc);
		}

		//color
		//{
		//	wgpu::TextureDescriptor desc;
		//	desc.label = "RendererColorTexture";
		//	desc.dimension = wgpu::TextureDimension::e2D;
		//	desc.format = GraphicsContext::GetSurfaceFormat();
		//	desc.mipLevelCount = 1;
		//	desc.sampleCount = 1;
		//	desc.size = { width, height, 1 };
		//	desc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;
		//	m_ColorTexture = engine::GraphicsContext::GetDevice().CreateTexture(&desc);
		//}

		//{
		//	wgpu::TextureViewDescriptor desc;
		//	desc.label = "RendererColorTextureView";
		//	desc.aspect = wgpu::TextureAspect::Undefined;
		//	desc.dimension = wgpu::TextureViewDimension::e2D;
		//	desc.format = m_ColorTexture.GetFormat();
		//	desc.baseMipLevel = 0;
		//	desc.mipLevelCount = 1;
		//	desc.baseArrayLayer = 0;
		//	desc.arrayLayerCount = 1;
		//	m_FrameContext.colorTarget = m_ColorTexture.CreateView(&desc);
		//}
	}

	void Renderer::SetEnvironment(Texture2D texture)
	{
		if (!texture) texture = RESOURCES::TEXTURE::HDR;

		m_FrameContext.environment = EnvironmentBaker::BakeEnvironment(texture);
		CreateEnvironmentBindGroup();
	}

	void Renderer::CreateEnvironmentUniformBuffer()
	{
		wgpu::BufferDescriptor desc;
		desc.label = "EnvironmentUniformBuffer";
		desc.size = sizeof(EnvironmentUniforms);
		desc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
		m_EnvironmentUniformBuffer = GraphicsContext::GetDevice().CreateBuffer(&desc);
	}

	void Renderer::CreateEnvironmentBindGroup()
	{
		std::array<wgpu::BindGroupEntry, 8> entries;

		// 0 - EnvironmentSampler
		{
			wgpu::SamplerDescriptor desc;
			desc.label = "EnvironmentSampler";
			desc.minFilter = wgpu::FilterMode::Linear;
			desc.magFilter = wgpu::FilterMode::Linear;
			desc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
			desc.maxAnisotropy = 1;
			desc.lodMinClamp = 0.0f;
			desc.lodMaxClamp = 32.0f;

			wgpu::Sampler envSampler = GraphicsContext::GetDevice().CreateSampler(&desc);
			entries[0].binding = 0;
			entries[0].sampler = envSampler;
		}

		// 1 - EnvironmentMap
		{
			entries[1].binding = 1;
			entries[1].textureView = m_FrameContext.environment.EnvironmentMap.GetTextureView();
		}

		// 2 - IrradianceMap
		{
			entries[2].binding = 2;
			entries[2].textureView = m_FrameContext.environment.IrradianceMap.GetTextureView();
		}

		// 3 - PrefilteredSpecularMap
		{
			entries[3].binding = 3;
			entries[3].textureView = m_FrameContext.environment.PrefilteredSpecularMap.GetTextureView();
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
			entries[5].buffer = m_EnvironmentUniformBuffer;
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
			entries[7].textureView = m_FrameContext.depthTextureArrayView;
		}

		wgpu::BindGroupDescriptor desc;
		desc.label = "EnvironmentBindGroup";
		desc.layout = RenderPipelineLayouts::GetEnvironmentLayout();
		desc.entryCount = entries.size();
		desc.entries = entries.data();
		m_FrameContext.environmentBindGroup = GraphicsContext::GetDevice().CreateBindGroup(&desc);
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

			m_FrameContext.depthTextureViews[i] = texture.CreateView(&viewDesc);
		}

		wgpu::TextureViewDescriptor arrayViewDesc;
		arrayViewDesc.format = format;
		arrayViewDesc.aspect = wgpu::TextureAspect::DepthOnly;
		arrayViewDesc.dimension = wgpu::TextureViewDimension::e2DArray;
		arrayViewDesc.baseMipLevel = 0;
		arrayViewDesc.mipLevelCount = 1;
		arrayViewDesc.baseArrayLayer = 0;
		arrayViewDesc.arrayLayerCount = s_ShadowCascadeCount;

		m_FrameContext.depthTextureArrayView = texture.CreateView(&arrayViewDesc);
	}

	void Renderer::RenderScene(const SceneAsset& scene, const CameraComponent& camera, wgpu::TextureView targetView)
	{
		m_FrameContext.camera = &camera;
		m_FrameContext.colorTarget = targetView;

		wgpu::Queue queue = GraphicsContext::GetQueue();

		ViewUniforms viewUniforms;
		viewUniforms.view = camera.viewMatrix;
		viewUniforms.projection = camera.projectionMatrix;
		queue.WriteBuffer(m_ViewUniformBuffer, 0, &viewUniforms, sizeof(viewUniforms));

		m_FrameContext.drawList = DrawList::CreateFromScene(scene);

		const std::vector<glm::mat4>& modelMatrices = m_FrameContext.drawList.GetTransforms();
		queue.WriteBuffer(m_ModelStorageBuffer, 0, modelMatrices.data(), modelMatrices.size() * sizeof(glm::mat4));

		wgpu::CommandEncoder encoder = GraphicsContext::GetDevice().CreateCommandEncoder();

		for(const auto& pass : m_Passes)
		{
			pass.Execute(encoder, m_FrameContext);
		}

		wgpu::CommandBuffer commandBuffer = encoder.Finish();
		queue.Submit(1, &commandBuffer);
	}
}