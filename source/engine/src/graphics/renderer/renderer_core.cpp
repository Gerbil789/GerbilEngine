#include "enginepch.h"
#include "engine/graphics/renderer/renderer_core.h"
#include "engine/graphics/graphics_context.h"
#include "engine/graphics/renderer/layouts.h"

namespace engine
{
	static wgpu::Buffer CreateViewUniformBuffer()
	{
		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.label = "ViewUniformBuffer";
		bufferDesc.size = sizeof(ViewUniforms);
		bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
		return graphicsContext::GetDevice().CreateBuffer(&bufferDesc);
	}

	static wgpu::BindGroup CreateViewBindGroup(wgpu::Buffer buffer)
	{
		wgpu::BindGroupEntry entry;
		entry.binding = 0;
		entry.buffer = buffer;
		entry.offset = 0;
		entry.size = sizeof(ViewUniforms);

		wgpu::BindGroupDescriptor desc;
		desc.label = "ViewBindGroup";
		desc.layout = Layouts::viewBindGroupLayout;
		desc.entryCount = 1;
		desc.entries = &entry;
		return graphicsContext::GetDevice().CreateBindGroup(&desc);
	}

	static wgpu::Buffer CreateModelStorageBuffer()
	{
		const uint64_t bufferSize = 1024 * 256 * sizeof(glm::mat4);
		wgpu::BufferDescriptor desc;
		desc.label = "ModelStorageBuffer";
		desc.size = bufferSize;
		desc.usage = wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst;
		return graphicsContext::GetDevice().CreateBuffer(&desc);
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
		desc.layout = Layouts::modelBindGroupLayout;
		desc.entryCount = 1;
		desc.entries = &entry;
		return graphicsContext::GetDevice().CreateBindGroup(&desc);
	}


	static wgpu::Buffer CreateEnvironmentUniformBuffer()
	{
		wgpu::BufferDescriptor desc;
		desc.label = "EnvironmentUniformBuffer";
		desc.size = sizeof(ShadowUniforms);
		desc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
		return graphicsContext::GetDevice().CreateBuffer(&desc);
	}

	static wgpu::BindGroup CreateEnvironmentBindGroup(FrameContext& frameContext, wgpu::Buffer& buffer)
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

			wgpu::Sampler envSampler = graphicsContext::GetDevice().CreateSampler(&desc);
			entries[0].binding = 0;
			entries[0].sampler = envSampler;
		}

		// 1 - EnvironmentMap
		{
			entries[1].binding = 1;
			entries[1].textureView = frameContext.environment.EnvironmentMap.textureView;
		}

		// 2 - IrradianceMap
		{
			entries[2].binding = 2;
			entries[2].textureView = frameContext.environment.IrradianceMap.textureView;
		}

		// 3 - PrefilteredSpecularMap
		{
			entries[3].binding = 3;
			entries[3].textureView = frameContext.environment.PrefilteredSpecularMap.textureView;
		}

		// 4 - BRDFIntMap
		{
			const Texture2D brdftexture = Texture2D::Create("resources/hdr/brdf_integration_map_ct_ggx.hdr");
			entries[4].binding = 4;
			entries[4].textureView = brdftexture.textureView;
		}

		// 5 - ShadowUniforms
		{
			entries[5].binding = 5;
			entries[5].buffer = buffer;
			entries[5].offset = 0;
			entries[5].size = sizeof(ShadowUniforms);
		}

		// 6 -ShadowSampler
		{
			wgpu::SamplerDescriptor desc;
			desc.label = "ShadowSampler";
			desc.compare = wgpu::CompareFunction::LessEqual;
			desc.minFilter = wgpu::FilterMode::Linear;
			desc.magFilter = wgpu::FilterMode::Linear;
			desc.maxAnisotropy = 1;
			wgpu::Sampler shadowSampler = graphicsContext::GetDevice().CreateSampler(&desc);

			entries[6].binding = 6;
			entries[6].sampler = shadowSampler;
		}

		// 7 - ShadowMap
		{
			entries[7].binding = 7;
			entries[7].textureView = frameContext.depthtextureArrayView;
		}

		wgpu::BindGroupDescriptor desc;
		desc.label = "EnvironmentBindGroup";
		desc.layout = Layouts::environmentBindGroupLayout;
		desc.entryCount = entries.size();
		desc.entries = entries.data();
		return graphicsContext::GetDevice().CreateBindGroup(&desc);
	}

	void RendererCore::Initialize(uint32_t width, uint32_t height)
	{
		m_viewUniformBuffer = CreateViewUniformBuffer();
		m_frameContext.viewBindGroup = CreateViewBindGroup(m_viewUniformBuffer);

		m_modelStorageBuffer = CreateModelStorageBuffer();
    m_frameContext.modelBindGroup = CreateModelBindGroup(m_modelStorageBuffer);

		CreateShadowTexture();
		m_environmentUniformBuffer = CreateEnvironmentUniformBuffer();
		SetEnvironment(texture::hdr);
		SetSize(width, height);
	}

	void RendererCore::SetSize(uint32_t width, uint32_t height)
	{
		wgpu::TextureDescriptor desc;
		desc.label = "rendererDepthTexture";
		desc.dimension = wgpu::TextureDimension::e2D;
		desc.format = wgpu::TextureFormat::Depth24Plus;
		desc.mipLevelCount = 1;
		desc.sampleCount = 1;
		desc.size = { width, height, 1 };
		desc.usage = wgpu::TextureUsage::RenderAttachment;
		m_depthtexture = engine::graphicsContext::GetDevice().CreateTexture(&desc);

		wgpu::TextureViewDescriptor viewDesc;
		viewDesc.label = "rendererDepthTextureView";
		viewDesc.aspect = wgpu::TextureAspect::DepthOnly;
		viewDesc.dimension = wgpu::TextureViewDimension::e2D;
		viewDesc.format = m_depthtexture.GetFormat();
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = 1;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 1;
		m_frameContext.depthTarget = m_depthtexture.CreateView(&viewDesc);
	}

	void RendererCore::SetEnvironment(TextureHandle texture)
	{
		m_frameContext.environment = BakeEnvironment(texture);
		m_frameContext.environmentBindGroup = CreateEnvironmentBindGroup(m_frameContext, m_environmentUniformBuffer);
	}

	void RendererCore::CreateShadowTexture()
	{
		wgpu::TextureFormat format = wgpu::TextureFormat::Depth24Plus;

		wgpu::TextureDescriptor textureDesc;
		textureDesc.dimension = wgpu::TextureDimension::e2D;
		textureDesc.format = format;
		textureDesc.mipLevelCount = 1;
		textureDesc.sampleCount = 1;
		textureDesc.size = { 1024, 1024, s_shadowCascadeCount };
		textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc;
		textureDesc.viewFormatCount = 0;
		textureDesc.viewFormats = nullptr;
		wgpu::Texture texture = graphicsContext::GetDevice().CreateTexture(&textureDesc);

		for (int i = 0; i < s_shadowCascadeCount; i++)
		{
			wgpu::TextureViewDescriptor viewDesc;
			viewDesc.format = format;
			viewDesc.aspect = wgpu::TextureAspect::DepthOnly;
			viewDesc.dimension = wgpu::TextureViewDimension::e2D;
			viewDesc.baseMipLevel = 0;
			viewDesc.mipLevelCount = 1;
			viewDesc.baseArrayLayer = i;
			viewDesc.arrayLayerCount = 1;

			m_frameContext.depthtextureViews[i] = texture.CreateView(&viewDesc);
		}

		wgpu::TextureViewDescriptor arrayViewDesc;
		arrayViewDesc.format = format;
		arrayViewDesc.aspect = wgpu::TextureAspect::DepthOnly;
		arrayViewDesc.dimension = wgpu::TextureViewDimension::e2DArray;
		arrayViewDesc.baseMipLevel = 0;
		arrayViewDesc.mipLevelCount = 1;
		arrayViewDesc.baseArrayLayer = 0;
		arrayViewDesc.arrayLayerCount = s_shadowCascadeCount;

		m_frameContext.depthtextureArrayView = texture.CreateView(&arrayViewDesc);
	}

	wgpu::CommandEncoder RendererCore::BeginFrame(const component::Camera* camera, const DrawList& drawList)
	{
		m_frameContext.drawList = drawList;
		m_frameContext.camera = camera;

		wgpu::Queue queue = graphicsContext::GetQueue();

		ViewUniforms viewUniforms;
		viewUniforms.view = camera->viewMatrix;
		viewUniforms.projection = camera->projectionMatrix;
		queue.WriteBuffer(m_viewUniformBuffer, 0, &viewUniforms, sizeof(viewUniforms));

		const std::vector<glm::mat4>& modelMatrices = m_frameContext.drawList.transforms;
		queue.WriteBuffer(m_modelStorageBuffer, 0, modelMatrices.data(), modelMatrices.size() * sizeof(glm::mat4));

		return graphicsContext::GetDevice().CreateCommandEncoder();
	}

	void RendererCore::EndFrame(wgpu::CommandEncoder encoder)
	{
		wgpu::CommandBuffer commandBuffer = encoder.Finish();
		graphicsContext::GetQueue().Submit(1, &commandBuffer);
	}
}