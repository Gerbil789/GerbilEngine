#pragma once

#include "Engine/Graphics/Shader.h"
#include <webgpu/webgpu_cpp.h>
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


		bool operator==(const PipelineSpecification& other) const
		{
			if (shaderId != other.shaderId ||
				topology != other.topology ||
				frontFace != other.frontFace ||
				cullMode != other.cullMode ||
				depthWrite != other.depthWrite ||
				depthCompare != other.depthCompare ||
				depthFormat != other.depthFormat)
			{
				return false;
			}

			if (layoutOverrides.size() != other.layoutOverrides.size()) return false;

			for (size_t i = 0; i < layoutOverrides.size(); ++i)
			{
				// Compare the raw WebGPU C-handles
				if (layoutOverrides[i].Get() != other.layoutOverrides[i].Get())
				{
					return false;
				}
			}

			return true;
		}

		// 2. Hash all properties to prevent cache collisions
		size_t Hash() const
		{
			size_t seed = 0;

			HashCombine(seed, static_cast<uint64_t>(shaderId));
			HashCombine(seed, static_cast<uint32_t>(topology));
			HashCombine(seed, static_cast<uint32_t>(frontFace));
			HashCombine(seed, static_cast<uint32_t>(cullMode));
			HashCombine(seed, static_cast<uint32_t>(depthWrite));
			HashCombine(seed, static_cast<uint32_t>(depthCompare));
			HashCombine(seed, static_cast<uint32_t>(depthFormat));

			for (const auto& layout : layoutOverrides)
			{
				// Hash the memory address of the Dawn C-handle
				HashCombine(seed, reinterpret_cast<uint64_t>(layout.Get()));
			}

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