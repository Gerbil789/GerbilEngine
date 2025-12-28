#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"
#include "Engine/Core/UUID.h"
#include <webgpu/webgpu.hpp>

namespace Engine
{
	class EntityIdPass : public RenderPass
	{
  public:
    EntityIdPass();
    virtual void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context, const DrawList& drawList) override;
    Engine::UUID ReadPixel(uint32_t x, uint32_t y);

  private:
    void Resize(uint32_t width, uint32_t height);
    void CreateBindGroupLayout();
    void CreatePipeline();

  private:
		wgpu::Texture m_Texture;
    wgpu::TextureView m_TextureView;

		uint32_t m_Width = 0;
		uint32_t m_Height = 0;

    wgpu::RenderPipeline m_EntityIdPipeline;
    wgpu::BindGroupLayout m_BindGroupLayout;
    wgpu::BindGroup m_BindGroup;
		wgpu::Buffer m_UniformBuffer;
	};
}