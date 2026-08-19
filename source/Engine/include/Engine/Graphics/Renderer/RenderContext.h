#pragma once

#include "Engine/Graphics/Renderer/DrawList.h"
#include "Engine/Graphics/Renderer/RenderUniforms.h"
#include "Engine/Graphics/Texture/Environment.h"
#include "Engine/Core/Components.h"
#include <array>

namespace Engine
{
  class SceneAsset;

  struct RenderContext
  {
		float width = 1.0f;
		float height = 1.0f;

    wgpu::TextureView colorTarget;
    wgpu::TextureView depthTarget;

		DrawList drawList;

    SceneAsset* scene = nullptr;
    CameraComponent cameraComponent;

    // View
    wgpu::BindGroup viewBindGroup = nullptr;
    wgpu::Buffer viewUniformBuffer = nullptr;

    // Model 
    wgpu::BindGroup modelBindGroup = nullptr;
    wgpu::Buffer modelStorageBuffer = nullptr;

		// Environment
    wgpu::BindGroup environmentBindGroup = nullptr;
		wgpu::Buffer environmentUniformBuffer = nullptr;
    Environment environment;

    // Shadow texture views
    std::array<wgpu::TextureView, s_ShadowCascadeCount> depthTextureViews;
    wgpu::TextureView depthTextureArrayView;
  };
}