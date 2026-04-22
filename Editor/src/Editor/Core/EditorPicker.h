#pragma once

#include "Engine/Core/UUID.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/Renderer/RenderContext.h"
#include <webgpu/webgpu-raii.hpp>

namespace Editor
{
  class EditorPicker
  {
  public:
    EditorPicker();
    ~EditorPicker();

    void Resize(uint32_t width, uint32_t height);
    Engine::Uuid Pick(uint32_t mouseX, uint32_t mouseY, const Engine::RenderContext& context);

  private:
    void CreateBindGroupLayout();
    void CreatePipeline();

  private:
    wgpu::Texture m_ColorTexture;
    wgpu::TextureView m_ColorTextureView;

    wgpu::Texture m_DepthTexture;
    wgpu::TextureView m_DepthTextureView;

    wgpu::Buffer m_ReadbackBuffer;
    wgpu::Buffer m_UniformBuffer;

    wgpu::RenderPipeline m_Pipeline;
    wgpu::BindGroupLayout m_BindGroupLayout;
    wgpu::BindGroup m_BindGroup;

    uint32_t m_Width = 0;
    uint32_t m_Height = 0;
  };
}