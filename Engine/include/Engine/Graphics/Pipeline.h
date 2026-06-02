#pragma once

#include "Engine/Graphics/Shader.h"
#include <webgpu/webgpu.hpp>
#include <unordered_map>

namespace Engine
{
  template <class T>
  inline void HashCombine(std::size_t& seed, const T& v)
  {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2); // The magic number 0x9e3779b9 is derived from the golden ratio
  } 

  struct PipelineSpecification
  {
    Shader* shader;
    wgpu::PrimitiveTopology topology = wgpu::PrimitiveTopology::TriangleList;
    wgpu::CullMode cullMode = wgpu::CullMode::Back;

    bool operator==(const PipelineSpecification& other) const = default;

    size_t Hash() const
    {
      size_t seed = 0;

      HashCombine(seed, shader->id);
      HashCombine(seed, static_cast<uint32_t>(topology));
      HashCombine(seed, static_cast<uint32_t>(cullMode));

      return seed;
    }
  };




  class PipelineCache
  {
  public:
    static wgpu::RenderPipeline GetOrCreatePipeline(const PipelineSpecification& specification);

  private:
		inline static std::unordered_map<size_t, wgpu::RenderPipeline> s_PipelineCache;
  };
}