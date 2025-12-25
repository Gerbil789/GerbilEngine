#pragma once

#include <webgpu/webgpu.hpp>

namespace Engine
{
  class Scene;
	class Camera;

  struct RenderContext
  {
    wgpu::TextureView colorTarget;
    wgpu::TextureView depthTarget;

    uint32_t width = 0;
    uint32_t height = 0;

    Scene* scene = nullptr;
    Camera* camera = nullptr;
  };
}