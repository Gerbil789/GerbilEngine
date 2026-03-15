#include "enginepch.h"
#include "Engine/Graphics/RenderPass/OpaquePass.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"
#include "Engine/Scene/Components.h"

namespace Engine
{
	void OpaquePass::Execute(wgpu::CommandEncoder& encoder, const RenderContext& context, const DrawList& drawList)
	{
		if (!m_Enabled) return;

		wgpu::RenderPassColorAttachment color{};
		color.view = context.colorTarget;
		color.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		color.loadOp = wgpu::LoadOp::Load;
		color.storeOp = wgpu::StoreOp::Store;
		color.clearValue = wgpu::Color(0.0f, 0.0f, 0.0f, 0.0f);

		wgpu::RenderPassDepthStencilAttachment depth{};
		depth.view = context.depthTarget;
		depth.depthClearValue = 1.0f;
		depth.depthLoadOp = wgpu::LoadOp::Clear;
		depth.depthStoreOp = wgpu::StoreOp::Store;
		depth.depthReadOnly = false;
		depth.stencilClearValue = 0;
		depth.stencilLoadOp = wgpu::LoadOp::Undefined;
		depth.stencilStoreOp = wgpu::StoreOp::Undefined;
		depth.stencilReadOnly = true;

		wgpu::RenderPassDescriptor passDescriptor{};
		passDescriptor.label = { "OpaqueRenderPass", WGPU_STRLEN };
		passDescriptor.colorAttachmentCount = 1;
		passDescriptor.colorAttachments = &color;
		passDescriptor.depthStencilAttachment = &depth;

		wgpu::RenderPassEncoder pass = encoder.beginRenderPass(passDescriptor);

		pass.setBindGroup(0, RenderGlobals::GetFrameBindGroup(), 0, nullptr);

		Material* material = nullptr;
		Mesh* mesh = nullptr;

		for (const DrawItem& draw : drawList.items)
		{
			if (draw.mesh == nullptr) continue;

			if (draw.mesh != mesh)
			{
				mesh = draw.mesh;
				pass.setVertexBuffer(0, mesh->GetVertexBuffer(), 0, mesh->GetVertexBuffer().getSize());
				pass.setIndexBuffer(mesh->GetIndexBuffer(), wgpu::IndexFormat::Uint32, 0, mesh->GetIndexBuffer().getSize());
			}
			const SubMesh* sub = draw.subMesh;

			auto meshMaterials = draw.entity.Get<MeshComponent>().materials;
			if(meshMaterials.size() < 1)
			{
				continue;
			}

			auto subMaterial = meshMaterials[sub->materialIndex];

			if (subMaterial && (subMaterial != material))
			{
				material = subMaterial;
				GraphicsContext::GetQueue().writeBuffer(material->GetUniformBuffer(), 0, material->GetUniformData().data(), material->GetUniformData().size());
				pass.setBindGroup(2, material->GetBindGroup(), 0, nullptr);
				pass.setPipeline(material->GetShader()->GetRenderPipeline());
			}

			uint32_t dynamicOffset = draw.modelIndex * RenderGlobals::GetModelUniformStride();
			pass.setBindGroup(1, RenderGlobals::GetModelBindGroup(), 1, &dynamicOffset);

			pass.drawIndexed(sub->indexCount, 1, sub->firstIndex, 0, 0);
		}

		pass.end();
	}
}