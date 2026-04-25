#pragma once

#include "Engine/Graphics/Renderer/DrawList.h"
#include "Engine/Graphics/Renderer/RenderUniforms.h"
#include "Engine/Graphics/Texture/Environment.h"
#include <webgpu/webgpu-raii.hpp>
#include <array>

namespace Engine
{
  class Scene;
	class TextureCube;
	class Camera;

  struct RenderContext
  {
    wgpu::TextureView colorTarget;
    wgpu::TextureView depthTarget;

		DrawList drawList;

    Scene* scene = nullptr;
    Camera* camera = nullptr;

    // View
    wgpu::BindGroup viewBindGroup = nullptr;
    wgpu::Buffer viewUniformBuffer = nullptr;

    // Model 
    wgpu::BindGroup modelBindGroup = nullptr;
    wgpu::Buffer modelUniformBuffer = nullptr;

		// Environment
    wgpu::BindGroup environmentBindGroup = nullptr;
		wgpu::Buffer environmentUniformBuffer = nullptr;
    Environment environment;

    // Shadow texture views
    std::array<wgpu::TextureView, s_ShadowCascadeCount> depthTextureViews;
    wgpu::TextureView depthTextureArrayView;
  };
}