#include "enginepch.h"
#include "RenderCommand.h"
#include "Engine/Core/Application.h"
#include <webgpu/webgpu.hpp>

namespace Engine 
{
	// handle resizing whole window
	void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		WGPUSurfaceConfiguration config = {};
		config.nextInChain = nullptr;
		config.device = Application::Get().GetGraphicsContext()->GetDevice();
		config.format = WGPUTextureFormat_RGBA8Unorm;
		config.usage = WGPUTextureUsage_RenderAttachment;
		config.width = width;
		config.height = height;
		config.presentMode = WGPUPresentMode_Fifo;
		config.alphaMode = WGPUCompositeAlphaMode_Opaque;
		config.viewFormatCount = 0;
		config.viewFormats = nullptr;

		wgpuSurfaceConfigure(Application::Get().GetGraphicsContext()->GetSurface(), &config);
	}
}