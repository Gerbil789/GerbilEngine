#include "enginepch.h"
#include "Engine/Graphics/Renderer/RenderPass.h"
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"

namespace engine
{
  void BackgroundPass::Execute(wgpu::CommandEncoder& encoder, const FrameContext& context)
  {
    PipelineSpecification spec;
    spec.shader = Shader::Skybox;
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
    desc.label = "BackgroundRenderPass";
    desc.colorAttachmentCount = 1;
    desc.colorAttachments = &color;

    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&desc);

    pass.SetBindGroup(0, context.viewBindGroup, 0, nullptr);

    if (context.camera->projectionType == CameraComponent::Projection::Perspective)
    {
      if (context.camera->backgroundMode == CameraComponent::Background::Skybox)
      {
        pass.SetPipeline(pipeline);
        pass.SetBindGroup(1, context.environmentBindGroup, 0, nullptr);
        pass.Draw(36, 1, 0, 0);
      }
    }

    pass.End();
  }
}