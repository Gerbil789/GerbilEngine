#pragma once

#include "engine/graphics/renderer/frame_context.h"
// #include <cstdint>

namespace engine
{
  struct DrawList;

  struct RendererCore
  {
    void Initialize(uint32_t width, uint32_t height);
    void SetSize(uint32_t width, uint32_t height);
    void SetEnvironment(TextureHandle texture);
    
    wgpu::CommandEncoder BeginFrame(const component::Camera* camera, const DrawList& drawList);
    void EndFrame(wgpu::CommandEncoder encoder, wgpu::TextureView targetView);

  private:
    void CreateEnvironmentUniformBuffer();
    void CreateEnvironmentBindGroup();
    void CreateShadowtexture();

  protected:
    wgpu::Texture m_depthtexture;
    wgpu::Texture m_colortexture;
    wgpu::Buffer m_viewUniformBuffer;
    wgpu::Buffer m_modelStorageBuffer;
    wgpu::Buffer m_environmentUniformBuffer;
    FrameContext m_frameContext;
  };
}