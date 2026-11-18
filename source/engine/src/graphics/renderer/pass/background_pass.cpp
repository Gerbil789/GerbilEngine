#include "enginepch.h"
#include "engine/graphics/renderer/render_pass.h"
#include "engine/graphics/renderer/render_pipeline_layouts.h"

namespace engine
{
  void Backgroundpass::Execute(wgpu::CommandEncoder& encoder, const FrameContext& context)
  {
    PipelineSpecification spec;
    spec.shaderHandle = shader::Skybox;
    spec.cullMode = wgpu::CullMode::None;
    spec.depthWrite = false;
    spec.layoutOverrides = { RenderPipelineLayouts::GetViewLayout(), RenderPipelineLayouts::GetEnvironmentLayout() };
    wgpu::RenderPipeline pipeline = PipelineCache::GetPipeline(spec);

    const glm::vec4& col = context.camera->clearColor;

    wgpu::RenderPassColorAttachment color;
    color.view = context.colorTarget;
    color.loadOp = wgpu::LoadOp::Clear;
    color.storeOp = wgpu::StoreOp::Store;
    color.clearValue = wgpu::Color{ col.r, col.g, col.b, col.a };

    wgpu::RenderPassDescriptor desc;
    desc.label = "BackgroundRenderpass";
    desc.colorAttachmentCount = 1;
    desc.colorAttachments = &color;

    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&desc);

    pass.SetBindGroup(0, context.viewBindGroup, 0, nullptr);

    if (context.camera->projection == component::Camera::Projection::Perspective)
    {
      if (context.camera->backgroundMode == component::Camera::Background::Skybox)
      {
        pass.SetPipeline(pipeline);
        pass.SetBindGroup(1, context.environmentBindGroup, 0, nullptr);
        pass.Draw(36, 1, 0, 0);
      }
    }

    pass.End();
  }
}