#include "enginepch.h"
#include "Engine/Graphics/RenderPass/OpaquePass.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Pipeline.h"
#include "Engine/Asset/AssetManager.h"

namespace Engine
{
	void OpaquePass::Execute(wgpu::CommandEncoder& encoder, const RenderContext& context)
	{
		wgpu::RenderPassColorAttachment color;
		color.view = context.colorTarget;
		color.loadOp = wgpu::LoadOp::Load;
		color.storeOp = wgpu::StoreOp::Store;
		color.clearValue = wgpu::Color(0.0f, 0.0f, 0.0f, 0.0f);

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

		wgpu::RenderPassDescriptor passDescriptor;
		passDescriptor.label = "OpaqueRenderPass";
		passDescriptor.colorAttachmentCount = 1;
		passDescriptor.colorAttachments = &color;
		passDescriptor.depthStencilAttachment = &depth;

		wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&passDescriptor);

		pass.SetBindGroup(0, context.viewBindGroup, 0, nullptr);
		pass.SetBindGroup(1, context.environmentBindGroup, 0, nullptr);
		pass.SetBindGroup(3, context.modelBindGroup, 0, nullptr);

		Mesh lastMesh;
		Material lastMaterial;

		for (const auto& [i, item] : std::views::enumerate(context.drawList.GetItems()))
		{
			if (item.mesh != lastMesh)
			{
				lastMesh = item.mesh;
				const MeshAsset& meshAsset = AssetManager::GetAsset(lastMesh);
				pass.SetVertexBuffer(0, meshAsset.GetVertexBuffer(), 0, meshAsset.GetVertexBuffer().GetSize());
				pass.SetIndexBuffer(meshAsset.GetIndexBuffer(), wgpu::IndexFormat::Uint32, 0, meshAsset.GetIndexBuffer().GetSize());
			}

			if (item.material != lastMaterial)
			{
				lastMaterial = item.material;
				const MaterialAsset& material = AssetManager::GetAsset(lastMaterial);
				GraphicsContext::GetQueue().WriteBuffer(material.GetUniformBuffer(), 0, material.GetUniformData().data(), material.GetUniformData().size());
				pass.SetBindGroup(2, material.GetBindGroup(), 0, nullptr);

				pass.SetPipeline(material.GetPipeline());
			}

			pass.DrawIndexed(item.indexCount, 1, item.firstIndex, 0, static_cast<uint32_t>(i));
		}

		pass.End();
	}
}