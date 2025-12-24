#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Graphics/Renderer/RenderContext.h"

namespace Engine
{
  class RenderPass
  {
  public:
    virtual ~RenderPass() = default;
    virtual void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context, Scene* scene, Camera* camera) = 0;
  };
}