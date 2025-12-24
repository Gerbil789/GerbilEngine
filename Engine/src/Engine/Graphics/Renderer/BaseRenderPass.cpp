#include "enginepch.h"
#include "BaseRenderPass.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"

namespace Engine
{
	BaseRenderPass::BaseRenderPass()
	{
	}

	BaseRenderPass::~BaseRenderPass()
	{
	}

	void BaseRenderPass::Execute(wgpu::CommandEncoder& encoder, const RenderContext& context, Scene* scene, Camera* camera)
	{
		ENGINE_PROFILE_FUNCTION();
		wgpu::RenderPassColorAttachment colorAttachment{};
		colorAttachment.view = context.colorTarget;
		colorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		colorAttachment.loadOp = wgpu::LoadOp::Clear;
		colorAttachment.storeOp = wgpu::StoreOp::Store;
		glm::vec4 col = camera->GetClearColor();
		colorAttachment.clearValue = wgpu::Color(col.r, col.g, col.b, col.a);

		wgpu::RenderPassDepthStencilAttachment depthStencilAttachment{};
		depthStencilAttachment.view = context.depthTarget;
		depthStencilAttachment.depthClearValue = 1.0f;
		depthStencilAttachment.depthLoadOp = wgpu::LoadOp::Clear;
		depthStencilAttachment.depthStoreOp = wgpu::StoreOp::Store;
		depthStencilAttachment.depthReadOnly = false;
		depthStencilAttachment.stencilClearValue = 0;
		depthStencilAttachment.stencilLoadOp = wgpu::LoadOp::Undefined;
		depthStencilAttachment.stencilStoreOp = wgpu::StoreOp::Undefined;
		depthStencilAttachment.stencilReadOnly = true;

		wgpu::RenderPassDescriptor renderPassDescriptor;
		renderPassDescriptor.label = { "RenderPass" };
		renderPassDescriptor.colorAttachmentCount = 1;
		renderPassDescriptor.colorAttachments = &colorAttachment;
		renderPassDescriptor.depthStencilAttachment = &depthStencilAttachment;

		wgpu::RenderPassEncoder pass = encoder.beginRenderPass(renderPassDescriptor);

		pass.setBindGroup(GroupID::Frame, RenderGlobals::GetFrameBindGroup(), 0, nullptr);


		if (camera->GetBackgroundType() == Camera::BackgroundType::Skybox)
		{
			// Skybox
			auto& skybox = camera->GetSkybox();
			pass.setPipeline(skybox.GetShader().GetRenderPipeline());
			pass.setBindGroup(1, skybox.GetBindGroup(), 0, nullptr);
			pass.draw(36, 1, 0, 0);
		}

		//glm::mat4 vp = m_Camera->GetViewProjectionMatrix();
		//Frustum frustum = ExtractFrustum(vp);

		const std::vector<Entity>& entities = scene->GetEntities<TransformComponent, MeshComponent>();
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

				glm::mat4 modelMatrix = entity.GetComponent<TransformComponent>().GetWorldMatrix(scene->GetRegistry());

				//AABB worldAABB = TransformAABB(mesh->GetBounds(), modelMatrix);

				//if (!IsVisible(frustum, worldAABB))
				//{
				//	continue;
				//}


				uint32_t dynamicOffset = i * RenderGlobals::GetModelUniformStride();
				GraphicsContext::GetQueue().writeBuffer(RenderGlobals::GetModelUniformBuffer(), dynamicOffset, &modelMatrix, sizeof(modelMatrix));
				pass.setBindGroup(GroupID::Model, RenderGlobals::GetModelBindGroup(), 1, &dynamicOffset);

				pass.setVertexBuffer(0, mesh->GetVertexBuffer(), 0, mesh->GetVertexBuffer().getSize());
				pass.setIndexBuffer(mesh->GetIndexBuffer(), wgpu::IndexFormat::Uint16, 0, mesh->GetIndexBuffer().getSize());
				pass.drawIndexed(mesh->GetIndexCount(), 1, 0, 0, 0);

				i++;
			}
		}
		//LOG_INFO("Rendered {0} objects", i);

		pass.end();
		pass.release();
	}
}