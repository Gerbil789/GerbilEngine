#pragma once

#include "Engine/Graphics/Renderer/RenderContext.h"

namespace Engine
{
  class RenderPass
  {
  public:
    virtual ~RenderPass() = default;
    virtual void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context) = 0;

		bool m_Enabled = true;
  };
}