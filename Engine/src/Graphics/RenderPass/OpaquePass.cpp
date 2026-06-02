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

		Engine::Uuid materialId{ 0 };
		Engine::Material* material = nullptr;
		Mesh* mesh = nullptr;

		for (const DrawItem& item : context.drawList)
		{
			if (item.mesh == nullptr) continue;

			if (item.mesh != mesh)
			{
				mesh = item.mesh;
				pass.setVertexBuffer(0, mesh->GetVertexBuffer(), 0, mesh->GetVertexBuffer().getSize());
				pass.setIndexBuffer(mesh->GetIndexBuffer(), wgpu::IndexFormat::Uint32, 0, mesh->GetIndexBuffer().getSize());
			}
			const SubMesh* sub = item.subMesh;

			auto meshMaterials = context.scene->GetRegistry().get<MeshComponent>(item.entity).materials;
			if(meshMaterials.size() < 1)
			{
				continue;
			}

			Engine::Uuid subMaterial;
			if(sub->materialIndex >= meshMaterials.size())
			{
				subMaterial = meshMaterials[0];
			}
			else
			{
				subMaterial = meshMaterials[sub->materialIndex];
			}

			if (subMaterial && (subMaterial != materialId))
			{
				materialId = subMaterial;
				material = &Engine::AssetManager::GetAsset<Material>(subMaterial);
				GraphicsContext::GetQueue().writeBuffer(material->GetUniformBuffer(), 0, material->GetUniformData().data(), material->GetUniformData().size());
				pass.setBindGroup(2, material->GetBindGroup(), 0, nullptr);

				wgpu::RenderPipeline pipeline = PipelineCache::GetOrCreatePipeline(material->GetPipelineSpec());
				pass.setPipeline(pipeline);
			}

			uint32_t dynamicOffset = item.modelIndex * GraphicsContext::GetUniformBufferOffsetAlignment();
			pass.setBindGroup(3, context.modelBindGroup, 1, &dynamicOffset);

			pass.drawIndexed(sub->indexCount, 1, sub->firstIndex, 0, 0);
		}

		pass.end();
	}
}