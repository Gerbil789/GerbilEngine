#pragma once

#include "Engine/Graphics/RenderPass/BackgroundPass.h"
#include "Engine/Graphics/RenderPass/OpaquePass.h"
#include "Engine/Graphics/RenderPass/WireframePass.h"
#include "Engine/Graphics/RenderPass/NormalPass.h"
#include "Engine/Graphics/RenderPass/LightPass.h"
#include "Engine/Graphics/RenderPass/ShadowPass.h"
#include <memory>
#include <unordered_map>

namespace Engine
{
  class RenderPassRegistry
  {
  public:
    static RenderPass* GetPass(RenderPassType type)
    {
      auto& passes = GetPasses(); // Get the map

      // Safety check: if the pass isn't in the map, return nullptr
      if (passes.find(type) == passes.end())
        return nullptr;

      return passes[type].get();
    }

  private:
    static std::unordered_map<RenderPassType, std::unique_ptr<RenderPass>>& GetPasses()
    {
      static std::unordered_map<RenderPassType, std::unique_ptr<RenderPass>> s_Passes;

      // If the map is empty, fill it up!
      if (s_Passes.empty())
      {
        s_Passes[RenderPassType::Background] = std::make_unique<BackgroundPass>();
        s_Passes[RenderPassType::Opaque] = std::make_unique<OpaquePass>();
        s_Passes[RenderPassType::Shadow] = std::make_unique<ShadowPass>();
        s_Passes[RenderPassType::Light] = std::make_unique<LightPass>();
        s_Passes[RenderPassType::Wireframe] = std::make_unique<WireframePass>();
        s_Passes[RenderPassType::Normal] = std::make_unique<NormalPass>();
      }
      return s_Passes;
    }
  };
}
