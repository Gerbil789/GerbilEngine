#pragma once

#include "Engine/Core/API.h"
#include <webgpu/webgpu.hpp>

namespace Engine::RenderGlobals
{
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
    glm::mat4 lightViewProj;
  };
  static_assert(sizeof(ShadowUniforms) % 16 == 0);


  ENGINE_API void Initialize();

  // Frame
  wgpu::BindGroupLayout GetFrameLayout();
  wgpu::BindGroup GetFrameBindGroup();
  wgpu::Buffer GetFrameUniformBuffer();

	wgpu::Buffer GetShadowUniformBuffer();

  // Model
  wgpu::BindGroupLayout GetModelLayout();
  wgpu::BindGroup GetModelBindGroup();
  wgpu::Buffer GetModelUniformBuffer();
  uint32_t GetModelUniformStride();
}