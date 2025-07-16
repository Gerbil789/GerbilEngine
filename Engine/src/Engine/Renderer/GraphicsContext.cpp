#include "enginepch.h"
#define WEBGPU_CPP_IMPLEMENTATION // must be defined before including webgpu.hpp
#include "GraphicsContext.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Application.h"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Engine::GraphicsContext
{
	wgpu::Device s_Device;
	wgpu::Queue s_Queue;
	wgpu::Surface s_Surface;

	//TODO: make these lambdas 
	static constexpr auto OnDeviceLost = [](WGPUDevice const* device, WGPUDeviceLostReason reason, WGPUStringView message, void* userdata1, void* userdata2) 
	{
		LOG_ERROR("WebGPU device lost. Reason: {}, Message: {}", (int)reason, message);
	};

	static constexpr auto OnUncapturedError = [](WGPUDevice const* device, WGPUErrorType type, WGPUStringView message, void* userdata1, void* userdata2) 
	{
		LOG_ERROR("WebGPU Uncaptured error: {}", message);
	};

	wgpu::Surface glfwGetWGPUSurface(wgpu::Instance instance, GLFWwindow* window)
	{
		wgpu::SurfaceSourceWindowsHWND hwndDesc;
		hwndDesc.chain.sType = WGPUSType_SurfaceSourceWindowsHWND;
		hwndDesc.hinstance = GetModuleHandle(NULL);
		hwndDesc.hwnd = glfwGetWin32Window(window);

		wgpu::SurfaceDescriptor surfaceDesc = {};
		surfaceDesc.nextInChain = &hwndDesc.chain;
		surfaceDesc.label = { "MainSurface", WGPU_STRLEN };

		return instance.createSurface(surfaceDesc);
	}

	void Initialize()
	{
		Engine::Window& window = Engine::Application::Get().GetWindow();

		// Initialize WGPU instance
		wgpu::InstanceDescriptor desc;
		desc.setDefault();
		wgpu::Instance instance = wgpu::createInstance(desc);
		ASSERT(instance, "Failed to create WGPU instance");

		// Create WGPU surface
		s_Surface = glfwGetWGPUSurface(instance, window.Get());
		ASSERT(s_Surface, "Failed to create WebGPU surface");

		// Request adapter
		wgpu::RequestAdapterOptions adapterOpts = {};
		adapterOpts.compatibleSurface = s_Surface;
		wgpu::Adapter adapter = instance.requestAdapter(adapterOpts);
		ASSERT(adapter, "Failed to request WGPU adapter");
		instance.release();
		
		// Request device
		wgpu::DeviceLostCallbackInfo deviceLostCallbackInfo = {};
		deviceLostCallbackInfo.callback = OnDeviceLost;

		wgpu::UncapturedErrorCallbackInfo uncapturedErrorCallbackInfo = {};
		uncapturedErrorCallbackInfo.callback = OnUncapturedError;

		wgpu::DeviceDescriptor deviceDesc = {};
		deviceDesc.label = { "MainDevice", WGPU_STRLEN };
		deviceDesc.requiredFeatureCount = 0;
		deviceDesc.defaultQueue.label = { "DefaultQueue", WGPU_STRLEN };
		deviceDesc.deviceLostCallbackInfo = deviceLostCallbackInfo;
		deviceDesc.uncapturedErrorCallbackInfo = uncapturedErrorCallbackInfo;
		s_Device = adapter.requestDevice(deviceDesc);
		ASSERT(s_Device, "Failed to request WebGPU device");

		// Get queue
		s_Queue = s_Device.getQueue();
		ASSERT(s_Queue, "Failed to get WGPU queue");

		// Configure surface
		wgpu::SurfaceConfiguration config = {};
		config.width = window.GetWidth();
		config.height = window.GetHeight(); 
		config.usage = WGPUTextureUsage_RenderAttachment;
		config.format = WGPUTextureFormat_RGBA8Unorm;
		config.viewFormatCount = 0;
		config.viewFormats = nullptr;
		config.device = s_Device;
		config.presentMode = WGPUPresentMode_Fifo;
		config.alphaMode = WGPUCompositeAlphaMode_Opaque;
		s_Surface.configure(config);

		adapter.release();
	}

	void GraphicsContext::Shutdown()
	{
		if (s_Surface) s_Surface.unconfigure();
		if (s_Queue) s_Queue.release();
		if (s_Surface) s_Surface.release();
		if (s_Device) s_Device.release();

		s_Device = nullptr;
		s_Queue = nullptr;
		s_Surface = nullptr;

		LOG_INFO("WGPU GraphicsContext shutdown");
	}

	void GraphicsContext::SetViewport(uint32_t width, uint32_t height)
	{
		WGPUSurfaceConfiguration config = {};
		config.nextInChain = nullptr;
		config.device = GraphicsContext::GetDevice();
		config.format = WGPUTextureFormat_RGBA8Unorm;
		config.usage = WGPUTextureUsage_RenderAttachment;
		config.width = width;
		config.height = height;
		config.presentMode = WGPUPresentMode_Fifo;
		config.alphaMode = WGPUCompositeAlphaMode_Opaque;
		config.viewFormatCount = 0;
		config.viewFormats = nullptr;

		wgpuSurfaceConfigure(s_Surface, &config);
	}



}