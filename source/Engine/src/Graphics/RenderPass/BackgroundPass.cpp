#include "enginepch.h"
#include "Engine/Graphics/RenderPass/BackgroundPass.h"
#include "Engine/Graphics/SkyboxShader.h"

namespace Engine
{
	namespace
	{
		SkyboxShader m_SkyboxShader; //TODO: use generic shared class..., delete skybox shader
	}

	BackgroundPass::BackgroundPass()
	{
		m_SkyboxShader.Initialize("resources/shaders/skybox.wgsl");
	}

	void BackgroundPass::Execute(wgpu::CommandEncoder& encoder, const RenderContext& context)
	{
		const glm::vec4& col = context.cameraComponent.clearColor;

		wgpu::RenderPassColorAttachment color;
		color.view = context.colorTarget;
		color.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		color.loadOp = wgpu::LoadOp::Clear;
		color.storeOp = wgpu::StoreOp::Store;
		color.clearValue = wgpu::Color{ col.r, col.g, col.b, col.a };

		wgpu::RenderPassDescriptor passDescriptor;
		passDescriptor.label = "BackgroundRenderPass";
		passDescriptor.colorAttachmentCount = 1;
		passDescriptor.colorAttachments = &color;

		wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&passDescriptor);

		pass.SetBindGroup(0, context.viewBindGroup, 0, nullptr);

		if (context.cameraComponent.projectionType == CameraComponent::Projection::Perspective)
		{
			if (context.cameraComponent.background == CameraComponent::Background::Skybox)
			{
				pass.SetPipeline(m_SkyboxShader.GetRenderPipeline());
				pass.SetBindGroup(1, context.environmentBindGroup, 0, nullptr);
				pass.Draw(36, 1, 0, 0);
			}
		}

		pass.End();
	}
}