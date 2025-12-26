#include "enginepch.h"
#define WEBGPU_CPP_IMPLEMENTATION // must be defined before including webgpu.hpp
#include "GraphicsContext.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Application.h"
#include "Engine/Graphics/WebGPUUtils.h"
#include <GLFW/glfw3.h>

//TODO: move platform specific code to separate files
#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#endif

#include <GLFW/glfw3native.h>


namespace Engine::GraphicsContext
{
	static wgpu::Instance s_Instance;
	static wgpu::Device s_Device;
	static wgpu::Queue s_Queue;
	static wgpu::Surface s_Surface;

	wgpu::Surface glfwGetWGPUSurface(wgpu::Instance instance, GLFWwindow* window)
	{
#if defined(_WIN32)

		wgpu::SurfaceSourceWindowsHWND hwndDesc{};
		hwndDesc.chain.sType = WGPUSType_SurfaceSourceWindowsHWND;
		hwndDesc.hinstance = GetModuleHandle(nullptr);
		hwndDesc.hwnd = glfwGetWin32Window(window);

		wgpu::SurfaceDescriptor surfaceDesc{};
		surfaceDesc.label = { "MainSurface", WGPU_STRLEN };
		surfaceDesc.nextInChain = &hwndDesc.chain;

		return instance.createSurface(surfaceDesc);

#elif defined(__linux__)

		wgpu::SurfaceSourceXlibWindow x11Desc{};
		x11Desc.chain.sType = WGPUSType_SurfaceSourceXlibWindow;
		x11Desc.display = glfwGetX11Display();
		x11Desc.window = glfwGetX11Window(window);

		wgpu::SurfaceDescriptor surfaceDesc{};
		surfaceDesc.label = { "MainSurface", WGPU_STRLEN };
		surfaceDesc.nextInChain = &x11Desc.chain;

		return instance.createSurface(surfaceDesc);

#endif
	}

	void Initialize()
	{
		Engine::Window& window = Engine::Application::GetWindow();

		// Initialize WGPU instance
		wgpu::InstanceDescriptor desc;
		desc.setDefault();
		desc.requiredFeatureCount = 1;
		desc.requiredFeatures = &wgpu::InstanceFeatureName::TimedWaitAny;
		s_Instance = wgpu::createInstance(desc);
		if(!s_Instance)
		{
			throw std::runtime_error("Failed to create WGPU instance");
		}

		// Create WGPU surface
		s_Surface = glfwGetWGPUSurface(s_Instance, static_cast<GLFWwindow*>(window.GetNativeWindow()));
		if(!s_Surface)
		{
			throw std::runtime_error("Failed to create WGPU surface");
		}

		// Request adapter
		wgpu::RequestAdapterOptions adapterOpts = {};
		adapterOpts.compatibleSurface = s_Surface;
		wgpu::Adapter adapter = s_Instance.requestAdapter(adapterOpts);
		if(!adapter)
		{
			throw std::runtime_error("Failed to request WGPU adapter");
		}

		// Print backend info
		{
			wgpu::AdapterInfo info;
			adapter.getInfo(&info);

			LOG_TRACE("Dawn backend: {}", BackendTypeToString(info.backendType));
			LOG_TRACE("GPU: {} ({})", info.device, info.architecture);
			LOG_TRACE("Description: {}", info.description);
			LOG_TRACE("VendorID 0x{:X}", info.vendorID);
			LOG_TRACE("DeviceID 0x{:X}", info.deviceID);
		}

		// Request device
		wgpu::DeviceDescriptor deviceDesc = {};
		deviceDesc.label = { "MainDevice", WGPU_STRLEN };
		deviceDesc.requiredFeatureCount = 0;
		deviceDesc.defaultQueue.label = { "DefaultQueue", WGPU_STRLEN };

		deviceDesc.deviceLostCallbackInfo.mode = wgpu::CallbackMode::AllowSpontaneous;
		deviceDesc.deviceLostCallbackInfo.callback = [](WGPUDevice const*, WGPUDeviceLostReason reason, WGPUStringView message, void*, void*)
			{
				if (reason == wgpu::DeviceLostReason::Destroyed) return; // ignore shutdown losses (explicit destroy)
				LOG_ERROR("WebGPU device lost. Reason: {}, Message: {}", (int)reason, message);
			};

		deviceDesc.uncapturedErrorCallbackInfo.callback = [](WGPUDevice const*, WGPUErrorType type, WGPUStringView message, void*, void*)
			{
				LOG_ERROR("WebGPU Uncaptured error [type: {}]: {}", ErrorTypeToString(type), message);
			};

		s_Device = adapter.requestDevice(deviceDesc);
		if(!s_Device)
		{
			throw std::runtime_error("Failed to request WGPU device");
		}

		adapter.release();

		// Get queue
		s_Queue = s_Device.getQueue();
		if(!s_Queue)
		{
			throw std::runtime_error("Failed to get WGPU queue");
		}

		// Configure surface
		ConfigureSurface(window.GetWidth(), window.GetHeight());
	}

	void Shutdown()
	{
		if (s_Surface) s_Surface.unconfigure();
		if (s_Queue) s_Queue.release();
		if (s_Surface) s_Surface.release();
		if (s_Device) s_Device.release();
		if (s_Instance) s_Instance.release();
	}

	void ConfigureSurface(uint32_t width, uint32_t height)
	{
		wgpu::SurfaceConfiguration config = {};
		config.width = width;
		config.height = height;
		config.device = s_Device;
		config.format = wgpu::TextureFormat::RGBA8Unorm;
		config.usage = wgpu::TextureUsage::RenderAttachment;
		config.presentMode = wgpu::PresentMode::Fifo;
		config.alphaMode = wgpu::CompositeAlphaMode::Opaque;
		config.viewFormatCount = 0;
		config.viewFormats = nullptr;
		config.nextInChain = nullptr;

		s_Surface.configure(config);
	}

	const wgpu::Instance& GetInstance()
	{
		return s_Instance;
	}

	const wgpu::Device& GetDevice()
	{
		return s_Device;
	}

	const wgpu::Queue& GetQueue()
	{
		return s_Queue;
	}

	const wgpu::Surface& GetSurface()
	{
		return s_Surface;
	}
}