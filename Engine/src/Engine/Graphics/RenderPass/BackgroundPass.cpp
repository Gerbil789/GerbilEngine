#include "enginepch.h"
#include "BackgroundPass.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"
#include "Engine/Graphics/Camera.h"

namespace Engine
{
	void BackgroundPass::Execute(wgpu::CommandEncoder& encoder, const RenderContext& context, const DrawList&)
	{
		wgpu::RenderPassColorAttachment color{};
		color.view = context.colorTarget;
		color.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		color.loadOp = wgpu::LoadOp::Clear;
		color.storeOp = wgpu::StoreOp::Store;
		const glm::vec4& col = context.camera->GetClearColor();
		color.clearValue = wgpu::Color(col.r, col.g, col.b, col.a);

		wgpu::RenderPassDescriptor passDescriptor;
		passDescriptor.label = { "BackgroundRenderPass", WGPU_STRLEN };
		passDescriptor.colorAttachmentCount = 1;
		passDescriptor.colorAttachments = &color;

		wgpu::RenderPassEncoder pass = encoder.beginRenderPass(passDescriptor);

		pass.setBindGroup(GroupID::Frame, RenderGlobals::GetFrameBindGroup(), 0, nullptr);

		if (context.camera->GetBackgroundType() == Camera::BackgroundType::Skybox)
		{
			auto& skybox = context.camera->GetSkybox();
			pass.setPipeline(skybox.GetShader().GetRenderPipeline());
			pass.setBindGroup(1, skybox.GetBindGroup(), 0, nullptr);
			pass.draw(36, 1, 0, 0);
		}
		pass.end();
	}
}