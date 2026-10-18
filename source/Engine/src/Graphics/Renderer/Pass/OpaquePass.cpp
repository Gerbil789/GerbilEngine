#include "enginepch.h"
#include "Engine/Graphics/Renderer/RenderPass.h"

namespace engine
{
  void OpaquePass::Execute(wgpu::CommandEncoder& encoder, const FrameContext& context)
  {
    wgpu::RenderPassColorAttachment color;
    color.view = context.colorTarget;
    color.loadOp = wgpu::LoadOp::Load;
    color.storeOp = wgpu::StoreOp::Store;
    color.clearValue = wgpu::Color{ 0.0f, 0.0f, 0.0f, 0.0f };

    wgpu::RenderPassDepthStencilAttachment depth;
    depth.view = context.depthTarget;
    depth.depthClearValue = 1.0f;
    depth.depthLoadOp = wgpu::LoadOp::Clear;
    depth.depthStoreOp = wgpu::StoreOp::Store;
    depth.depthReadOnly = false;
    depth.stencilClearValue = 0;
    depth.stencilLoadOp = wgpu::LoadOp::Undefined;
    depth.stencilStoreOp = wgpu::StoreOp::Undefined;
    depth.stencilReadOnly = true;

    wgpu::RenderPassDescriptor desc;
    desc.label = "OpaqueRenderPass";
    desc.colorAttachmentCount = 1;
    desc.colorAttachments = &color;
    desc.depthStencilAttachment = &depth;

    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&desc);

    pass.SetBindGroup(0, context.viewBindGroup, 0, nullptr);
    pass.SetBindGroup(1, context.environmentBindGroup, 0, nullptr);
    pass.SetBindGroup(3, context.modelBindGroup, 0, nullptr);

    Mesh currentMesh = Mesh::Empty;
    MaterialInstance currentMaterialInstance = MaterialInstance::Null;

    uint32_t i = 0;
    for (const auto& item : context.drawList.items)
    {
      if (item.mesh != currentMesh)
      {
        currentMesh = item.mesh;
        const MeshAsset& MeshAsset = currentMesh.Get();
        pass.SetVertexBuffer(0, MeshAsset.vertexBuffer, 0, MeshAsset.vertexBuffer.GetSize());
        pass.SetIndexBuffer(MeshAsset.indexBuffer, wgpu::IndexFormat::Uint32, 0, MeshAsset.indexBuffer.GetSize());
      }

      if (item.materialInstance != currentMaterialInstance)
      {
        currentMaterialInstance = item.materialInstance;
        MaterialInstanceData data = currentMaterialInstance.Get();

        GraphicsContext::GetQueue().WriteBuffer(data.uniformBuffer, 0, data.uniformData.data(), data.uniformData.size());
        pass.SetBindGroup(2, data.bindGroup, 0, nullptr);
        pass.SetPipeline(data.material.GetPipeline());
      }

      pass.DrawIndexed(item.indexCount, 1, item.firstIndex, 0, i++);
    }

    pass.End();
  }
}