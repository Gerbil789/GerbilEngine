#include "enginepch.h"
#define WEBGPU_CPP_IMPLEMENTATION // must be defined before including webgpu.hpp
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/WebGPUUtils.h"

namespace Engine::GraphicsContext
{
	static wgpu::Instance s_Instance;
	static wgpu::Device s_Device;
	static wgpu::Queue s_Queue;

	static uint32_t s_UniformBufferOffsetAlignment;
	static uint32_t s_StorageBufferOffsetAlignment;

	void Initialize()
	{
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

		// Request adapter
		wgpu::RequestAdapterOptions adapterOpts{};
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
			LOG_TRACE("GPU: {} ({})", ToStringView(info.device), ToStringView(info.architecture));
			LOG_TRACE("Description: {}", ToStringView(info.description));
			LOG_TRACE("VendorID 0x{:X}", info.vendorID);
			LOG_TRACE("DeviceID 0x{:X}", info.deviceID);
		}

		wgpu::Limits limits;

		limits.maxBufferSize = 1024ull * 1024ull * 1024ull; // request 1 GB

		if (adapter.getLimits(&limits) != wgpu::Status::Success)
		{
			throw std::runtime_error("Failed to query adapter limits");
		}

		s_UniformBufferOffsetAlignment = limits.minUniformBufferOffsetAlignment;
		s_StorageBufferOffsetAlignment = limits.minStorageBufferOffsetAlignment;

		// Request device
		wgpu::DeviceDescriptor deviceDesc{};
		deviceDesc.label = { "MainDevice", WGPU_STRLEN };
		deviceDesc.requiredFeatureCount = 0;
		deviceDesc.defaultQueue.label = { "DefaultQueue", WGPU_STRLEN };
		deviceDesc.requiredLimits = &limits;

		deviceDesc.deviceLostCallbackInfo.mode = wgpu::CallbackMode::AllowSpontaneous;
		deviceDesc.deviceLostCallbackInfo.callback = [](WGPUDevice const*, WGPUDeviceLostReason reason, WGPUStringView message, void*, void*)
			{
				if (reason == wgpu::DeviceLostReason::Destroyed) return; // ignore shutdown losses (explicit destroy)
				LOG_ERROR("WebGPU device lost. Reason: {}, Message: {}", (int)reason, message.data);
			};

		deviceDesc.uncapturedErrorCallbackInfo.callback = [](WGPUDevice const*, WGPUErrorType type, WGPUStringView message, void*, void*)
			{
				LOG_ERROR("WebGPU Uncaptured error [type: {}]: {}", ErrorTypeToString(type), message.data);
			};

		deviceDesc.requiredFeatureCount = 1;
		wgpu::FeatureName features[]
		{
			wgpu::FeatureName::Float32Filterable
		};
		deviceDesc.requiredFeatures = (WGPUFeatureName*)features;

		s_Device = adapter.requestDevice(deviceDesc);
		if(!s_Device)
		{
			throw std::runtime_error("Failed to request WGPU device");
		}

		bool hasFloat32Filterable = s_Device.hasFeature(wgpu::FeatureName::Float32Filterable);
		LOG_WARNING("Float32Filterable: {}", hasFloat32Filterable ? "YES" : "NO");

		adapter.release();

		// Get queue
		s_Queue = s_Device.getQueue();
		if(!s_Queue)
		{
			throw std::runtime_error("Failed to get WGPU queue");
		}
	}

	void Shutdown()
	{
		if (s_Queue) s_Queue.release();
		if (s_Device) s_Device.release();
		if (s_Instance) s_Instance.release();
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

	ENGINE_API uint32_t GetUniformBufferOffsetAlignment()
	{
		return s_UniformBufferOffsetAlignment;
	}

	ENGINE_API uint32_t GetStorageBufferOffsetAlignment()
	{
		return s_StorageBufferOffsetAlignment;
	}
}