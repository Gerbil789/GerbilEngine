#include "enginepch.h"
#include "BackgroundPass.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Graphics/Camera.h"

namespace Engine
{
	BackgroundPass::BackgroundPass()
	{
	}

	BackgroundPass::~BackgroundPass()
	{
	}

	void BackgroundPass::Execute(wgpu::CommandEncoder& encoder, const RenderContext& context)
	{
		wgpu::RenderPassColorAttachment color{};
		color.view = context.colorTarget;
		color.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		color.loadOp = wgpu::LoadOp::Clear;
		color.storeOp = wgpu::StoreOp::Store;
		const glm::vec4& col = context.camera->GetClearColor();
		color.clearValue = wgpu::Color(col.r, col.g, col.b, col.a);

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

		wgpu::RenderPassDescriptor passDescriptor;
		passDescriptor.label = { "BackgroundRenderPass" };
		passDescriptor.colorAttachmentCount = 1;
		passDescriptor.colorAttachments = &color;
		passDescriptor.depthStencilAttachment = &depth;

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
		pass.release();
	}
}