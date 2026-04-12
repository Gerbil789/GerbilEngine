#pragma once

#include <webgpu/webgpu.hpp>
#include <array>

namespace Engine
{
  constexpr static int s_ShadowCascadeCount = 4;

  class Scene;
	class Camera;
	class CubeMapTexture;

  struct RenderContext
  {
    wgpu::TextureView colorTarget;
    wgpu::TextureView depthTarget;

    uint32_t width = 0;
    uint32_t height = 0;

    Scene* scene = nullptr;
    Camera* camera = nullptr;

    CubeMapTexture* environmentCubemap = nullptr;

    // View
    wgpu::BindGroup viewBindGroup = nullptr;
    wgpu::Buffer viewUniformBuffer = nullptr;

    // Model 
    wgpu::BindGroup modelBindGroup = nullptr;
    wgpu::Buffer modelUniformBuffer = nullptr;

		// Environment
    wgpu::BindGroup environmentBindGroup = nullptr;
		wgpu::Buffer environmentUniformBuffer = nullptr;

    // Shadow texture views
    std::array<wgpu::TextureView, s_ShadowCascadeCount> depthTextureViews;
    wgpu::TextureView depthTextureArrayView;
  };
}