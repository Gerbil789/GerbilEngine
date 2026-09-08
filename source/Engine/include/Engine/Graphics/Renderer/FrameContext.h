#pragma once

#include "Engine/Graphics/Renderer/DrawList.h"
#include "Engine/Graphics/Renderer/RenderUniforms.h"
#include "Engine/Graphics/Texture/Environment.h"
#include "Engine/Core/Components.h"

namespace engine
{
  struct FrameContext
  {
    wgpu::TextureView colorTarget;
    wgpu::TextureView depthTarget;

		DrawList drawList;

    const CameraComponent* camera;
    //EnvironmentComponent* environmentComponent;

    wgpu::BindGroup viewBindGroup;
    wgpu::BindGroup modelBindGroup;
    wgpu::BindGroup environmentBindGroup ;

    Environment environment;

    std::array<wgpu::TextureView, s_ShadowCascadeCount> depthTextureViews;
    wgpu::TextureView depthTextureArrayView;
  };
}