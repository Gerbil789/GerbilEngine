#pragma once

#include "Engine/Graphics/Renderer/FrameContext.h"

namespace engine
{
  struct RenderPass
  {
    wgpu::RenderPipeline pipeline;
    std::function<void(wgpu::CommandEncoder& encoder, const FrameContext& context)> Execute;
  };

  namespace pass
  {
    const RenderPass Background();
    const RenderPass Shadow();
    const RenderPass Opaque();
    const RenderPass UI();
    const RenderPass Normal();
    const RenderPass Wireframe();
  }
}