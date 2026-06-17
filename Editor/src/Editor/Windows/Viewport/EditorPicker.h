#pragma once

#include "Engine/Core/UUID.h"
#include <webgpu/webgpu.hpp>

namespace Editor
{
  class EditorPicker
  {
  public:
    ~EditorPicker();

		void Initialize();

    void Resize(uint32_t width, uint32_t height);
    Engine::Uuid Pick(uint32_t mouseX, uint32_t mouseY);

  private:
    void CreateBindGroupLayout();
    void CreatePipeline();

  private:
    wgpu::Texture m_ColorTexture;
    wgpu::TextureView m_ColorTextureView;

    wgpu::Texture m_DepthTexture;
    wgpu::TextureView m_DepthTextureView;

    wgpu::Buffer m_ReadbackBuffer;
    wgpu::Buffer m_IdStorageBuffer;

    wgpu::RenderPipeline m_Pipeline;
    wgpu::BindGroupLayout m_BindGroupLayout;
    wgpu::BindGroup m_BindGroup;

    uint32_t m_Width = 0;
    uint32_t m_Height = 0;
  };
}