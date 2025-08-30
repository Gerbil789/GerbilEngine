#pragma once

#include "Engine/Scene/Scene.h"
#include <webgpu/webgpu.hpp>

namespace Engine
{
	class EntityIdRenderer
	{
  public:
    EntityIdRenderer(uint32_t width, uint32_t height);

    void Resize(uint32_t width, uint32_t height);
    void SetScene(Scene* scene) { m_Scene = scene; }
    void Render();

    wgpu::TextureView GetTextureView() const { return m_TextureView; }
    Engine::UUID ReadPixel(uint32_t x, uint32_t y);

  private:
    void CreateBindGroupLayout();
    void CreatePipeline();

  private:
    Scene* m_Scene;
		wgpu::Device m_Device;
		wgpu::Queue m_Queue;

		wgpu::Texture m_Texture;
    wgpu::TextureView m_TextureView;
    wgpu::TextureView m_DepthView;

    wgpu::RenderPipeline m_Pipeline;
    wgpu::BindGroupLayout m_BindGroupLayout;
    wgpu::BindGroup m_BindGroup;
		wgpu::Buffer m_UniformBuffer;

    wgpu::TextureView m_EntityIdView;
	};
}