#include "enginepch.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"
#include "Engine/Graphics/Renderer/DrawList.h"
#include <execution>

namespace Engine
{
	Renderer::~Renderer()
	{
		for (auto pass : m_Passes)
		{
			delete pass;
		}
		m_Passes.clear();
	}

	void Renderer::AddPass(RenderPass* pass)
	{
		m_Passes.push_back(pass);
	}

	void Renderer::RemovePass(RenderPass* pass)
	{
		auto it = std::find(m_Passes.begin(), m_Passes.end(), pass);
		if(it != m_Passes.end())
		{
			m_Passes.erase(it);
		}
	}


	void Renderer::SetScene(Scene* scene)
	{
		m_RenderContext.scene = scene;
	}

	void Renderer::SetCamera(Camera* camera)
	{
		m_RenderContext.camera = camera;
	}

	void Renderer::Resize(uint32_t width, uint32_t height)
	{
		m_RenderContext.width = width;
		m_RenderContext.height = height;

		// Color
		{
			wgpu::TextureDescriptor desc{};
			desc.label = { "RendererColorTexture", WGPU_STRLEN };
			desc.dimension = wgpu::TextureDimension::_2D;
			desc.format = wgpu::TextureFormat::RGBA8Unorm;
			desc.size = { width, height, 1 };
			desc.mipLevelCount = 1;
			desc.sampleCount = 1;
			desc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;
			wgpu::Texture colorTexture = GraphicsContext::GetDevice().createTexture(desc);

			wgpu::TextureViewDescriptor view{};
			view.label = { "RendererColorTextureView", WGPU_STRLEN };
			view.dimension = wgpu::TextureViewDimension::_2D;
			view.format = desc.format;
			view.baseMipLevel = 0;
			view.mipLevelCount = 1;
			view.baseArrayLayer = 0;
			view.arrayLayerCount = 1;
			m_RenderContext.colorTarget = colorTexture.createView(view);
		}

		// Depth
		{
			wgpu::TextureDescriptor desc;
			desc.label = { "RendererDepthTextureView", WGPU_STRLEN };
			desc.dimension = wgpu::TextureDimension::_2D;
			desc.format = wgpu::TextureFormat::Depth24Plus;
			desc.mipLevelCount = 1;
			desc.sampleCount = 1;
			desc.size = { width, height, 1 };
			desc.usage = wgpu::TextureUsage::RenderAttachment;
			desc.viewFormatCount = 1;
			desc.viewFormats = &wgpu::TextureFormat::Depth24Plus;
			wgpu::Texture depthTexture = GraphicsContext::GetDevice().createTexture(desc);

			wgpu::TextureViewDescriptor view;
			view.aspect = wgpu::TextureAspect::DepthOnly;
			view.baseArrayLayer = 0;
			view.arrayLayerCount = 1;
			view.baseMipLevel = 0;
			view.mipLevelCount = 1;
			view.dimension = wgpu::TextureViewDimension::_2D;
			view.format = wgpu::TextureFormat::Depth24Plus;
			m_RenderContext.depthTarget = depthTexture.createView(view);
		}
	}

	void Renderer::RenderScene()
	{
		//TODO: Move frame uniforms somewhere else...
		RenderGlobals::FrameUniforms frameUniforms;
		frameUniforms.view = m_RenderContext.camera->GetViewMatrix();
		frameUniforms.projection = m_RenderContext.camera->GetProjectionMatrix();
		frameUniforms.cameraPosition = m_RenderContext.camera->GetPosition();
		GraphicsContext::GetQueue().writeBuffer(RenderGlobals::GetFrameUniformBuffer(), 0, &frameUniforms, sizeof(frameUniforms));

		wgpu::CommandEncoderDescriptor encoderDesc{};
		encoderDesc.label = { "RendererCommandEncoder", WGPU_STRLEN };
		wgpu::CommandEncoder encoder = GraphicsContext::GetDevice().createCommandEncoder(encoderDesc);

		const DrawList& list = DrawList::CreateFromScene(m_RenderContext.scene);

		std::vector<glm::mat4> models(list.items.size());

		std::for_each(std::execution::par, list.items.begin(), list.items.end(), [&](const DrawItem& item)
			{
				models[item.modelIndex] = item.entity.Get<TransformComponent>().GetWorldMatrix();
			});

		for (const DrawItem& item : list.items)
		{
			uint32_t offset = item.modelIndex * RenderGlobals::GetModelUniformStride();
			GraphicsContext::GetQueue().writeBuffer(RenderGlobals::GetModelUniformBuffer(), offset, &models[item.modelIndex], sizeof(glm::mat4));
		}

		for(auto pass : m_Passes)
		{
			pass->Execute(encoder, m_RenderContext, list);
		}

		wgpu::CommandBuffer commandBuffer = encoder.finish();
		GraphicsContext::GetQueue().submit(1, &commandBuffer);
	}

	wgpu::TextureView Renderer::GetTextureView() const
	{
		return m_RenderContext.colorTarget;
	}

	void Renderer::SetColorTarget(wgpu::TextureView color)
	{
		m_RenderContext.colorTarget = color;
	}
}