#pragma once

#include "Engine/Graphics/Renderer/RenderContext.h"
#include "Engine/Graphics/Renderer/DrawList.h"

namespace Engine
{
  class RenderPass
  {
  public:
    virtual ~RenderPass() = default;
    virtual void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context, const DrawList& drawList) = 0;

		bool m_Enabled = true;
  };
}