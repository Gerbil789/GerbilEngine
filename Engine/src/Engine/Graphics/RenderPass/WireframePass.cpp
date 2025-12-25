#include "enginepch.h"
#include "WireframePass.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"
#include "Engine/Asset/AssetManager.h"

namespace Engine
{
	WireframePass::WireframePass()
	{
	}

	WireframePass::~WireframePass()
	{
	}

	void WireframePass::Execute(wgpu::CommandEncoder& encoder, const RenderContext& context) 
	{
		wgpu::RenderPassColorAttachment color{};
		color.view = context.colorTarget;
		color.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		color.loadOp = wgpu::LoadOp::Load;
		color.storeOp = wgpu::StoreOp::Store;

		wgpu::RenderPassDepthStencilAttachment depth{};
		depth.view = context.depthTarget;
		depth.depthClearValue = 1.0f;
		depth.depthLoadOp = wgpu::LoadOp::Load;
		depth.depthStoreOp = wgpu::StoreOp::Store;
		depth.depthReadOnly = false;
		depth.stencilClearValue = 0;
		depth.stencilLoadOp = wgpu::LoadOp::Undefined;
		depth.stencilStoreOp = wgpu::StoreOp::Undefined;
		depth.stencilReadOnly = true;

		wgpu::RenderPassDescriptor passDescriptor{};
		passDescriptor.label = { "WireframeRenderPass" };
		passDescriptor.colorAttachmentCount = 1;
		passDescriptor.colorAttachments = &color;
		passDescriptor.depthStencilAttachment = &depth;

		wgpu::RenderPassEncoder pass = encoder.beginRenderPass(passDescriptor);

		pass.setBindGroup(GroupID::Frame, RenderGlobals::GetFrameBindGroup(), 0, nullptr);

		const std::vector<Entity>& entities = context.scene->GetEntities<TransformComponent, MeshComponent>();
		std::unordered_map<Material*, std::vector<Entity>> materialGroups;

		// Group entities by material
		for (auto entity : entities)
		{
			auto& meshComponent = entity.GetComponent<MeshComponent>();
			if (meshComponent.mesh == nullptr)
			{
				continue;
			}

			Material* material = meshComponent.material;

			if (material == nullptr)
			{
				material = Material::GetDefault().get();
			}

			materialGroups[material].push_back(entity);
		}

		uint32_t i = 0;
		for (const auto& [material, groupEntities] : materialGroups)
		{
			Shader* shader = material->GetShader();
			material->Bind(pass);
			pass.setPipeline(shader->GetRenderPipeline());

			for (auto entity : groupEntities)
			{
				auto& meshComponent = entity.GetComponent<MeshComponent>();
				auto& mesh = meshComponent.mesh;

				glm::mat4 modelMatrix = entity.GetComponent<TransformComponent>().GetWorldMatrix(context.scene->GetRegistry());

				uint32_t dynamicOffset = i * RenderGlobals::GetModelUniformStride();
				GraphicsContext::GetQueue().writeBuffer(RenderGlobals::GetModelUniformBuffer(), dynamicOffset, &modelMatrix, sizeof(modelMatrix));
				pass.setBindGroup(GroupID::Model, RenderGlobals::GetModelBindGroup(), 1, &dynamicOffset);

				pass.setVertexBuffer(0, mesh->GetVertexBuffer(), 0, mesh->GetVertexBuffer().getSize());
				pass.setIndexBuffer(mesh->GetIndexBuffer(), wgpu::IndexFormat::Uint16, 0, mesh->GetIndexBuffer().getSize());
				pass.drawIndexed(mesh->GetIndexCount(), 1, 0, 0, 0);

				i++;
			}
		}

		pass.end();
		pass.release();
	}
}