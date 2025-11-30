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
		Engine::Window& window = Engine::Application::GetWindow();

		// Initialize WGPU instance
		wgpu::InstanceDescriptor desc;
		desc.setDefault();

		desc.requiredFeatureCount = 1;
		desc.requiredFeatures = &wgpu::InstanceFeatureName::TimedWaitAny;
		//desc.capabilities.timedWaitAnyEnable = true;
		s_Instance = wgpu::createInstance(desc);
		ASSERT(s_Instance, "Failed to create WGPU instance");

		// Create WGPU surface
		s_Surface = glfwGetWGPUSurface(s_Instance, window.GetNativeWindow());
		ASSERT(s_Surface, "Failed to create WebGPU surface");

		// Request adapter
		wgpu::RequestAdapterOptions adapterOpts = {};
		adapterOpts.compatibleSurface = s_Surface;
		wgpu::Adapter adapter = s_Instance.requestAdapter(adapterOpts);
		ASSERT(adapter, "Failed to request WGPU adapter");

		// print backend
		{
			wgpu::AdapterInfo info;
			adapter.getInfo(&info);

			switch (info.backendType)
			{
			case wgpu::BackendType::D3D11:  LOG_INFO("Dawn backend: D3D11"); break;
			case wgpu::BackendType::D3D12:  LOG_INFO("Dawn backend: D3D12"); break;
			case wgpu::BackendType::Metal:  LOG_INFO("Dawn backend: Metal"); break;
			case wgpu::BackendType::Vulkan: LOG_INFO("Dawn backend: Vulkan"); break;
			case wgpu::BackendType::OpenGL: LOG_INFO("Dawn backend: OpenGL"); break;
			case wgpu::BackendType::OpenGLES: LOG_INFO("Dawn backend: OpenGLES"); break;
			default: LOG_WARNING("Dawn backend: Unknown"); break;
			}

			LOG_INFO("GPU: {} ({})", info.device, info.architecture);
			LOG_INFO("Description: {}", info.description);
			LOG_INFO("VendorID {}", info.vendorID);
			LOG_INFO("DeviceID {}", info.deviceID);
		}

		// Request device
		wgpu::DeviceLostCallbackInfo deviceLostCallbackInfo = {};
		deviceLostCallbackInfo.mode = wgpu::CallbackMode::AllowSpontaneous;
		deviceLostCallbackInfo.callback = [](WGPUDevice const* device, WGPUDeviceLostReason reason, WGPUStringView message, void* userdata1, void* userdata2)
			{
				if (reason == wgpu::DeviceLostReason::Destroyed) return; // ignore shutdown
				LOG_ERROR("WebGPU device lost. Reason: {}, Message: {}", (int)reason, message);
			};

		wgpu::UncapturedErrorCallbackInfo uncapturedErrorCallbackInfo = {};
		uncapturedErrorCallbackInfo.callback = [](WGPUDevice const* device, WGPUErrorType type, WGPUStringView message, void* userdata1, void* userdata2) {
			LOG_ERROR("WebGPU Uncaptured error: {}", message);
			};

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
		if (s_Instance) s_Instance.release();
	}

	void GraphicsContext::SetWindowSize(uint32_t width, uint32_t height)
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