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
}