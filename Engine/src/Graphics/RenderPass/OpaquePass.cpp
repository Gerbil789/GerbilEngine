#include "enginepch.h"
#include "Engine/Graphics/RenderPass/OpaquePass.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/Pipeline.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Asset/AssetManager.h"

namespace Engine
{
	void OpaquePass::Execute(wgpu::CommandEncoder& encoder, const RenderContext& context)
	{
		wgpu::RenderPassColorAttachment color;
		color.view = context.colorTarget;
		color.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
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
		passDescriptor.label = { "OpaqueRenderPass", WGPU_STRLEN };
		passDescriptor.colorAttachmentCount = 1;
		passDescriptor.colorAttachments = &color;
		passDescriptor.depthStencilAttachment = &depth;

		wgpu::RenderPassEncoder pass = encoder.beginRenderPass(passDescriptor);

		pass.setBindGroup(0, context.viewBindGroup, 0, nullptr);
		pass.setBindGroup(1, context.environmentBindGroup, 0, nullptr);
		pass.setBindGroup(3, context.modelBindGroup, 0, nullptr);

		Engine::Uuid lastMeshId{};
		Engine::Uuid lastMaterialId{};

		for (const auto& [i, item] : std::views::enumerate(context.drawList))
		{
			if (item.meshId != lastMeshId)
			{
				lastMeshId = item.meshId;
				const Mesh& meshAsset = Engine::AssetManager::GetAsset<Mesh>(lastMeshId);
				pass.setVertexBuffer(0, meshAsset.GetVertexBuffer(), 0, meshAsset.GetVertexBuffer().getSize());
				pass.setIndexBuffer(meshAsset.GetIndexBuffer(), wgpu::IndexFormat::Uint32, 0, meshAsset.GetIndexBuffer().getSize());
			}

			if (item.materialId != lastMaterialId)
			{
				lastMaterialId = item.materialId;
				const Material& material = Engine::AssetManager::GetAsset<Material>(lastMaterialId);
				GraphicsContext::GetQueue().writeBuffer(material.GetUniformBuffer(), 0, material.GetUniformData().data(), material.GetUniformData().size());
				pass.setBindGroup(2, material.GetBindGroup(), 0, nullptr);

				//TODO: cache pipeline
				wgpu::RenderPipeline pipeline = PipelineCache::GetOrCreatePipeline(material.GetPipelineSpec());
				pass.setPipeline(pipeline);
			}

			pass.drawIndexed(item.indexCount, 1, item.firstIndex, 0, static_cast<uint32_t>(i));
		}

		pass.end();
	}
}