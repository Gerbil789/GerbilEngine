#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Renderer/Camera.h"
#include <webgpu/webgpu.hpp>

namespace Engine
{
  class RenderPass
  {
  public:
    virtual ~RenderPass() = default;
    virtual void Render(wgpu::CommandEncoder& encoder, Scene* scene, Camera* camera) = 0;
  };
}