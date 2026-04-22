#pragma once

#include "Engine/Core/API.h"
#include "Engine/Graphics/Renderer/RenderContext.h"
#include "Engine/Graphics/Renderer/DrawList.h"

namespace Engine
{
  class ENGINE_API RenderPass
  {
  public:
    virtual ~RenderPass() = default;
    virtual void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context) = 0;
  };

  enum class ENGINE_API RenderPassType : uint32_t
  {
    None = 0,
    Background = 1 << 0,
    Opaque = 1 << 1,
    Wireframe = 1 << 2,
    Normal = 1 << 3,
    Light = 1 << 4,
    Shadow = 1 << 5,
  };

  // Bitwise NOT
  inline RenderPassType operator~(RenderPassType a)
  {
    return static_cast<RenderPassType>(~static_cast<uint32_t>(a));
  }

  // Bitwise AND
  inline RenderPassType operator&(RenderPassType a, RenderPassType b)
  {
    return static_cast<RenderPassType>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
  }

  // Bitwise AND-Assignment
  inline RenderPassType& operator&=(RenderPassType& a, RenderPassType b)
  {
    a = a & b;
    return a;
  }

  // Bitwise OR and OR-Assignment
  inline RenderPassType operator|(RenderPassType a, RenderPassType b)
  {
    return static_cast<RenderPassType>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
  }

  // Bitwise OR-Assignment
  inline RenderPassType& operator|=(RenderPassType& a, RenderPassType b)
  {
    a = a | b;
    return a;
  }
}