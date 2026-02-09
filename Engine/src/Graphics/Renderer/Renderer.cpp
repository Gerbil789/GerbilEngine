#include "enginepch.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"
#include "Engine/Graphics/Renderer/DrawList.h"

namespace Engine
{
	Renderer::Renderer()
	{
		Resize(1, 1);
	}

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
		ENGINE_PROFILE_FUNCTION();

		m_RenderContext.width = width;
		m_RenderContext.height = height;

		// Color
		{
			wgpu::TextureDescriptor color{};
			color.label = { "RendererColorTexture", WGPU_STRLEN };
			color.dimension = wgpu::TextureDimension::_2D;
			color.format = wgpu::TextureFormat::RGBA8Unorm;
			color.size = { width, height, 1 };
			color.mipLevelCount = 1;
			color.sampleCount = 1;
			color.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;
			wgpu::Texture colorTexture = GraphicsContext::GetDevice().createTexture(color);

			wgpu::TextureViewDescriptor view{};
			view.label = { "RendererColorTextureView", WGPU_STRLEN };
			view.dimension = wgpu::TextureViewDimension::_2D;
			view.format = color.format;
			view.baseMipLevel = 0;
			view.mipLevelCount = 1;
			view.baseArrayLayer = 0;
			view.arrayLayerCount = 1;
			m_RenderContext.colorTarget = colorTexture.createView(view);
		}

		// Depth
		{
			wgpu::TextureDescriptor depth;
			depth.label = { "RendererDepthTextureView", WGPU_STRLEN };
			depth.dimension = wgpu::TextureDimension::_2D;
			depth.format = wgpu::TextureFormat::Depth24Plus;
			depth.mipLevelCount = 1;
			depth.sampleCount = 1;
			depth.size = { width, height, 1 };
			depth.usage = wgpu::TextureUsage::RenderAttachment;
			depth.viewFormatCount = 1;
			depth.viewFormats = &wgpu::TextureFormat::Depth24Plus;
			wgpu::Texture depthTexture = GraphicsContext::GetDevice().createTexture(depth);

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
		ENGINE_PROFILE_FUNCTION();

		//TODO: Move frame uniforms somewhere else...
		RenderGlobals::FrameUniforms frameUniforms;
		frameUniforms.view = m_RenderContext.camera->GetViewMatrix();
		frameUniforms.projection = m_RenderContext.camera->GetProjectionMatrix();
		frameUniforms.cameraPosition = m_RenderContext.camera->GetPosition();
		GraphicsContext::GetQueue().writeBuffer(RenderGlobals::GetFrameUniformBuffer(), 0, &frameUniforms, sizeof(frameUniforms));

		wgpu::CommandEncoderDescriptor encoderDesc = {};
		encoderDesc.label = { "RendererCommandEncoder", WGPU_STRLEN };
		wgpu::CommandEncoder encoder = GraphicsContext::GetDevice().createCommandEncoder(encoderDesc);

		const DrawList& list = DrawList::CreateFromScene(m_RenderContext.scene);

		for (const DrawItem& item : list.items)
		{
			glm::mat4 model = item.entity.GetComponent<TransformComponent>().GetWorldMatrix(m_RenderContext.scene->Registry());

			uint32_t offset = item.modelIndex * RenderGlobals::GetModelUniformStride();

			GraphicsContext::GetQueue().writeBuffer(RenderGlobals::GetModelUniformBuffer(), offset, &model, sizeof(glm::mat4));
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