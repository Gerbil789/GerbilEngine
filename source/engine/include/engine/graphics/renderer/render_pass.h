#pragma once

#include "engine/graphics/renderer/frame_context.h"

namespace engine
{
  struct IRenderpass
  {
    virtual ~IRenderpass() = default;
    virtual void Initialize() {};
    virtual void Execute(wgpu::CommandEncoder& encoder, const FrameContext& context) = 0;

    bool enabled = true;
  };

  struct Backgroundpass : public IRenderpass
  {
    void Execute(wgpu::CommandEncoder& encoder, const FrameContext& context) override;
  };

  struct Opaquepass : public IRenderpass
  {
    void Execute(wgpu::CommandEncoder& encoder, const FrameContext& context) override;
  };
}