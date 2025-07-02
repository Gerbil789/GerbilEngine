#include "enginepch.h"
#include "GraphicsContext.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Application.h"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>


namespace Engine
{
	static void RequestAdapterCallback(
		WGPURequestAdapterStatus status,
		WGPUAdapter adapter,
		WGPUStringView message,
		void* userdata1,
		void* userdata2
	)
	{
		if (status == WGPURequestAdapterStatus_Success)
		{
			*reinterpret_cast<WGPUAdapter*>(userdata1) = adapter;
		}
		else
		{
			LOG_ERROR("Adapter request failed: {}", message);
		}
	}

	static void RequestDeviceCallback(
		WGPURequestDeviceStatus status,
		WGPUDevice device,
		WGPUStringView message,
		void* userdata1,
		void* userdata2)
	{
		if (status == WGPURequestDeviceStatus_Success) {
			*reinterpret_cast<WGPUDevice*>(userdata1) = device;
			LOG_INFO("WGPU Device acquired successfully");
		}
		else {
			LOG_ERROR("Failed to get WGPU Device: {}", message);
		}
	}


	static void HandleDeviceLost(
		WGPUDevice const* device, 
		WGPUDeviceLostReason reason, 
		WGPUStringView message, 
		void* userdata1, 
		void* userdata2)
	{
		LOG_ERROR("WebGPU device lost. Reason: {}, Message: {}", (int)reason, message);
	}

	static void HandleUncapturedError(
		WGPUDevice const* device, 
		WGPUErrorType type, 
		WGPUStringView message, 
		void* userdata1, 
		void* userdata2)
	{
		LOG_ERROR("Uncaptured error: {}", message);
	}


	void GraphicsContext::Init()
	{
		Engine::Application& app = Engine::Application::Get();
		
		// Initialize WGPU instance
		WGPUInstanceDescriptor instanceDesc = {};
		instanceDesc.nextInChain = nullptr;
		m_Instance = wgpuCreateInstance(&instanceDesc);

		ASSERT(m_Instance, "Failed to create WGPU instance");
		LOG_INFO("WGPU instance created: {:#018x}", reinterpret_cast<uintptr_t>(m_Instance));

		// Create surface
		{
			HWND hwnd = glfwGetWin32Window(app.GetWindow().GetGLFWWindow());
			HINSTANCE hinstance = GetModuleHandle(NULL);

			WGPUSurfaceSourceWindowsHWND hwndDesc;
			hwndDesc.chain.next = nullptr;
			hwndDesc.chain.sType = WGPUSType_SurfaceSourceWindowsHWND;
			hwndDesc.hinstance = hinstance;
			hwndDesc.hwnd = hwnd;

			WGPUSurfaceDescriptor surfaceDesc = {};
			surfaceDesc.nextInChain = &hwndDesc.chain;
			surfaceDesc.label = { "MainSurface", strlen("MainSurface") };

			m_Surface = wgpuInstanceCreateSurface(m_Instance, &surfaceDesc);
			ASSERT(m_Surface, "Failed to create WebGPU surface");
			LOG_INFO("WGPU surface created: {:#018x}", reinterpret_cast<uintptr_t>(m_Surface));
		}

		// Request adapter
		WGPURequestAdapterOptions adapterOpts = {};
		adapterOpts.nextInChain = nullptr;
		adapterOpts.compatibleSurface = m_Surface;


		WGPURequestAdapterCallbackInfo adapterCallbackInfo = {};
		adapterCallbackInfo.mode = WGPUCallbackMode_AllowSpontaneous;
		adapterCallbackInfo.callback = RequestAdapterCallback;
		adapterCallbackInfo.userdata1 = &m_Adapter;

		wgpuInstanceRequestAdapter(m_Instance, &adapterOpts, adapterCallbackInfo);
		ASSERT(m_Adapter, "Failed to request WebGPU adapter");
		LOG_INFO("WGPU adapter obtained");
		wgpuInstanceRelease(m_Instance); // not needed anymore


		// Request device
		WGPUDeviceLostCallbackInfo deviceLostCallbackInfo = {};
		deviceLostCallbackInfo.nextInChain = nullptr;
		deviceLostCallbackInfo.callback = HandleDeviceLost; 

		WGPUUncapturedErrorCallbackInfo uncapturedErrorCallbackInfo = {};
		uncapturedErrorCallbackInfo.nextInChain = nullptr;
		uncapturedErrorCallbackInfo.callback = HandleUncapturedError;



		WGPUDeviceDescriptor deviceDesc = {};
		deviceDesc.nextInChain = nullptr;
		deviceDesc.label = { "MainDevice", strlen("MainDevice") };
		deviceDesc.requiredFeatureCount = 0; // we do not require any specific feature
		deviceDesc.requiredLimits = nullptr; // we do not require any specific limit
		deviceDesc.defaultQueue.nextInChain = nullptr;
		deviceDesc.defaultQueue.label = { "DefaultQueue", strlen("DefaultQueue") };
		deviceDesc.deviceLostCallbackInfo = deviceLostCallbackInfo;
		deviceDesc.uncapturedErrorCallbackInfo = uncapturedErrorCallbackInfo;

		WGPURequestDeviceCallbackInfo deviceCallbackInfo = {};
		deviceCallbackInfo.mode = WGPUCallbackMode_AllowSpontaneous;
		deviceCallbackInfo.callback = RequestDeviceCallback;
		deviceCallbackInfo.userdata1 = &m_Device;

		wgpuAdapterRequestDevice(m_Adapter, &deviceDesc, deviceCallbackInfo);
		ASSERT(m_Device, "Failed to request WebGPU device");
		LOG_INFO("WGPU device obtained");


		// Get queue
		m_Queue = wgpuDeviceGetQueue(m_Device);
		ASSERT(m_Queue, "Failed to get WGPU queue");
		LOG_INFO("WGPU queue obtained");


		// Configure surface
		WGPUSurfaceConfiguration config = {};
		config.nextInChain = nullptr;
		config.width = app.GetWindow().GetWidth();
		config.height = app.GetWindow().GetHeight(); //TODO: Handle window resizing
		config.usage = WGPUTextureUsage_RenderAttachment;
		config.format = WGPUTextureFormat_RGBA8Unorm;
		config.viewFormatCount = 0;
		config.viewFormats = nullptr;
		config.device = m_Device;
		config.presentMode = WGPUPresentMode_Fifo;
		config.alphaMode = WGPUCompositeAlphaMode_Opaque;
		wgpuSurfaceConfigure(m_Surface, &config);

		wgpuAdapterRelease(m_Adapter); // not needed anymore
	}


	void GraphicsContext::Shutdown()
	{
		if (m_Surface) wgpuSurfaceUnconfigure(m_Surface);
		if (m_Queue) wgpuQueueRelease(m_Queue);
		if (m_Surface) wgpuSurfaceRelease(m_Surface);
		if (m_Device) wgpuDeviceRelease(m_Device);
		if (m_Adapter) wgpuAdapterRelease(m_Adapter);
		if (m_Instance) wgpuInstanceRelease(m_Instance);

		m_Device = nullptr;
		m_Adapter = nullptr;
		m_Instance = nullptr;
		m_Queue = nullptr;
		m_Surface = nullptr;

		LOG_INFO("WGPU GraphicsContext shutdown");
	}
}