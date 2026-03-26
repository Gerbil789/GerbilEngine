#pragma once

#include "Engine/Core/API.h"
#include <webgpu/webgpu.hpp>

namespace Engine::RenderGlobals
{
  constexpr int shadowCascadeCount = 4;

  inline uint32_t UniformStride;
  inline uint32_t StorageStride;

  //TODO: i dont like having this in header...
  struct alignas(16) FrameUniforms {
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 cameraPosition;
    float padding;
  };
  static_assert(sizeof(FrameUniforms) % 16 == 0);

  struct alignas(16) ShadowUniforms
  {
		std::array<glm::mat4, shadowCascadeCount> lightViewProj;
		std::array<float, shadowCascadeCount> cascadeSplits;

  };
  static_assert(sizeof(ShadowUniforms) % 16 == 0);

  extern ShadowUniforms s_ShadowUniforms;


  ENGINE_API void Initialize();

  // Shadow
	wgpu::TextureView GetShadowTextureView(uint32_t cascadeIndex);
  wgpu::Buffer GetShadowUniformBuffer();

  // Frame
  wgpu::BindGroupLayout GetFrameLayout();
  wgpu::BindGroup GetFrameBindGroup();
  wgpu::Buffer GetFrameUniformBuffer();

  // Model
  wgpu::BindGroupLayout GetModelLayout();
  wgpu::BindGroup GetModelBindGroup();
  wgpu::Buffer GetModelUniformBuffer();
}