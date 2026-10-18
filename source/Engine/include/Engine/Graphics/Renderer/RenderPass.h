#pragma once

#include "Engine/Graphics/Renderer/FrameContext.h"

namespace engine
{
  struct IRenderPass
  {
    virtual ~IRenderPass() = default;
    virtual void Initialize() {};
    virtual void Execute(wgpu::CommandEncoder& encoder, const FrameContext& context) = 0;

    bool enabled = true;
  };


  struct BackgroundPass : public IRenderPass
  {
    void Execute(wgpu::CommandEncoder& encoder, const FrameContext& context) override;
  };

  struct OpaquePass : public IRenderPass
  {
    void Execute(wgpu::CommandEncoder& encoder, const FrameContext& context) override;
  };
}