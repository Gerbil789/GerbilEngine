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
		Uuid shaderId;
    wgpu::PrimitiveTopology topology = wgpu::PrimitiveTopology::TriangleList;
		wgpu::FrontFace frontFace = wgpu::FrontFace::CW;
    wgpu::CullMode cullMode = wgpu::CullMode::Back;

    bool depthWrite = true;
    wgpu::CompareFunction depthCompare = wgpu::CompareFunction::Less;
    wgpu::TextureFormat depthFormat = wgpu::TextureFormat::Depth24Plus;

		std::vector<wgpu::BindGroupLayout> layoutOverrides; // if empty, use the shader's default layout


    bool operator==(const PipelineSpecification& other) const = default;

    size_t Hash() const
    {
      size_t seed = 0;

      HashCombine(seed, static_cast<uint64_t>(shaderId));
      HashCombine(seed, static_cast<uint32_t>(topology));
      HashCombine(seed, static_cast<uint32_t>(cullMode));

      return seed;
    }
  };


  struct PipelineSpecHasher
  {
    size_t operator()(const PipelineSpecification& spec) const
    {
      return spec.Hash();
    }
  };

  class PipelineCache
  {
  public:
    static wgpu::RenderPipeline GetPipeline(const PipelineSpecification& specification);

  private:
		inline static std::unordered_map<PipelineSpecification, wgpu::RenderPipeline, PipelineSpecHasher> s_PipelineCache;
  };
}