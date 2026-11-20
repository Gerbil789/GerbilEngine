#pragma once

#include "engine/graphics/renderer/frame_context.h"

namespace engine
{
  struct BackgroundPass
  {
    bool enabled = true;
    void Execute(wgpu::CommandEncoder& encoder, const FrameContext& context);
  };

  struct OpaquePass
  {
    bool enabled = true;
    void Execute(wgpu::CommandEncoder& encoder, const FrameContext& context);
  };

  struct NormalPass
  {
    bool enabled = true;
    void Initialize();
    void Execute(wgpu::CommandEncoder& encoder, const FrameContext& context);
  };

  struct WireframePass
  {
    glm::vec4 color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    bool enabled = true;
    void Initialize();
    void Execute(wgpu::CommandEncoder& encoder, const FrameContext& context);
  };
}