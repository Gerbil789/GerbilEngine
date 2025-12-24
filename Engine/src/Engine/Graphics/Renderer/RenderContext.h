#pragma once

#include <webgpu/webgpu.hpp>

namespace Engine
{
  struct RenderContext
  {
    wgpu::TextureView colorTarget;
    wgpu::TextureView depthTarget;

    uint32_t width;
    uint32_t height;
  };
}