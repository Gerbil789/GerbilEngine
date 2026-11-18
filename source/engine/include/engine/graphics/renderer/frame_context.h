#pragma once

#include "engine/graphics/renderer/draw_list.h"
#include "engine/graphics/renderer/render_uniforms.h"
#include "engine/graphics/texture/environment.h"
#include "engine/core/components.h"

namespace engine
{
  struct FrameContext
  {
    wgpu::TextureView colorTarget;
    wgpu::TextureView depthTarget;

		DrawList drawList;

    const component::Camera* camera;

    wgpu::BindGroup viewBindGroup;
    wgpu::BindGroup modelBindGroup;
    wgpu::BindGroup environmentBindGroup ;

    Environment environment;

    std::array<wgpu::TextureView, s_shadowCascadeCount> depthtextureViews;
    wgpu::TextureView depthtextureArrayView;
  };
}