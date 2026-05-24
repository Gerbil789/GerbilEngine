#include "EditorRenderer.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Core/Window.h"
#include "Engine/Core/Log.h"
#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

namespace Editor
{
	static wgpu::Surface s_Surface;

	void EditorRenderer::Initialize(const Engine::Window& window)
	{
		s_Surface = wgpu::Surface(window.GetSurface());

		ImGui_ImplGlfw_InitForOther(static_cast<GLFWwindow*>(window.GetNativeWindow()), true);

		ImGui_ImplWGPU_InitInfo initInfo;
		initInfo.Device = Engine::GraphicsContext::GetDevice();
		initInfo.RenderTargetFormat = static_cast<WGPUTextureFormat>(window.GetSurfaceFormat());
		initInfo.DepthStencilFormat = wgpu::TextureFormat::Undefined;
		ImGui_ImplWGPU_Init(&initInfo);
	}

	void EditorRenderer::Shutdown()
	{
		ImGui_ImplWGPU_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}

	void EditorRenderer::BeginFrame()
	{
		ImGui_ImplWGPU_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void EditorRenderer::EndFrame()
	{
		ImGui::Render();

		wgpu::SurfaceTexture surfaceTexture;
		s_Surface.getCurrentTexture(&surfaceTexture);
		if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal)
		{
			LOG_ERROR("Failed to get current surface texture. status: {}", (int)surfaceTexture.status);
			return;
		}

		wgpu::Texture texture = surfaceTexture.texture;
		wgpu::TextureView targetView = texture.createView();

		if (!targetView)
		{
			LOG_ERROR("Failed to create texture view for surface texture");
			return;
		}

		wgpu::CommandEncoderDescriptor encoderDesc;
		encoderDesc.label = { "ImGuiCommandEncoderDescriptor", WGPU_STRLEN };
		wgpu::CommandEncoder encoder = Engine::GraphicsContext::GetDevice().createCommandEncoder(encoderDesc);

		wgpu::RenderPassColorAttachment color;
		color.view = targetView;
		color.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		color.loadOp = wgpu::LoadOp::Clear;
		color.storeOp = wgpu::StoreOp::Store;
		color.clearValue = wgpu::Color{ 0.9, 0.1, 0.2, 1.0 };

		wgpu::RenderPassDescriptor passDesc;
		passDesc.label = { "ImGuiRenderPassDescriptor", WGPU_STRLEN };
		passDesc.colorAttachmentCount = 1;
		passDesc.colorAttachments = &color;

		wgpu::RenderPassEncoder pass = encoder.beginRenderPass(passDesc);
		ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), pass);
		pass.end();

		wgpu::CommandBufferDescriptor commandBufferDesc;
		commandBufferDesc.label = { "ImGuiCommandBufferDescriptor", WGPU_STRLEN };
		commandBufferDesc.nextInChain = nullptr;
		wgpu::CommandBuffer commandBuffer = encoder.finish(commandBufferDesc);

		Engine::GraphicsContext::GetQueue().submit(1, &commandBuffer);
		s_Surface.present();
	}
}