#include "enginepch.h"
#include "Engine/Graphics/Renderer/RenderPass.h"
#include "Engine/Graphics/Pipeline.h"
#include "Engine/Asset/Resources.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"

namespace engine::pass
{
	const RenderPass Background()
	{
		PipelineSpecification spec;
		spec.shader = RESOURCES::SHADER::SKYBOX;
		spec.cullMode = wgpu::CullMode::None;
		spec.depthWrite = false;
		spec.layoutOverrides = { RenderPipelineLayouts::GetViewLayout(), RenderPipelineLayouts::GetEnvironmentLayout() };

		RenderPass renderPass;
		renderPass.pipeline = PipelineCache::GetPipeline(spec);

		renderPass.Execute = [=](wgpu::CommandEncoder& encoder, const FrameContext& context)
			{
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
						pass.SetPipeline(renderPass.pipeline);
						pass.SetBindGroup(1, context.environmentBindGroup, 0, nullptr);
						pass.Draw(36, 1, 0, 0);
					}
				}

				pass.End();
			};
		return renderPass;
	};


	const RenderPass Shadow()
	{
		return RenderPass{};
	}

	const RenderPass Opaque()
	{
		RenderPass renderPass;
		renderPass.pipeline = nullptr;
		renderPass.Execute = [](wgpu::CommandEncoder& encoder, const FrameContext& context)
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

				Mesh lastMesh;
				Material lastMaterial;

				size_t i = 0;
				for (const auto& item : context.drawList.GetItems())
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
					i++;
				}

				pass.End();
			};
		return renderPass;
	}

	const RenderPass UI()
	{
		return RenderPass{};
	}

	const RenderPass Normal()
	{
		return RenderPass{};
	}

	const RenderPass Wireframe()
	{
		return RenderPass{};
	}
}