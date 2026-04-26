#include "enginepch.h"
#define WEBGPU_CPP_IMPLEMENTATION // must be defined before including webgpu.hpp
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/WebGPUUtils.h"
#include "Engine/Graphics/SamplerPool.h"
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"

namespace Engine::GraphicsContext
{
	namespace
	{
		wgpu::raii::Instance s_Instance;
		wgpu::raii::Device s_Device;
		wgpu::raii::Queue s_Queue;

		uint32_t s_UniformBufferOffsetAlignment;
		uint32_t s_StorageBufferOffsetAlignment;
	}

	void Initialize()
	{
		wgpu::InstanceDescriptor desc;
		desc.setDefault();

		std::array<wgpu::InstanceFeatureName, 1> features = { wgpu::InstanceFeatureName::TimedWaitAny };
		desc.requiredFeatureCount = features.size();
		desc.requiredFeatures = reinterpret_cast<WGPUInstanceFeatureName*>(features.data());

		s_Instance = std::move(wgpu::createInstance(desc));
		if(!s_Instance)
		{
			throw std::runtime_error("Failed to create WGPU instance");
		}

		wgpu::RequestAdapterOptions adapterOpts;
		wgpu::raii::Adapter adapter = std::move(s_Instance->requestAdapter(adapterOpts));
		if(!adapter)
		{
			throw std::runtime_error("Failed to request WGPU adapter");
		}

		{
			wgpu::AdapterInfo info;
			adapter->getInfo(&info);

			LOG_TRACE("Dawn backend: {}", BackendTypeToString(info.backendType));
			LOG_TRACE("GPU: {} ({})", ToStringView(info.device), ToStringView(info.architecture));
			LOG_TRACE("Description: {}", ToStringView(info.description));
			LOG_TRACE("VendorID 0x{:X}", info.vendorID);
			LOG_TRACE("DeviceID 0x{:X}", info.deviceID);
		}

		wgpu::Limits limits;
		//limits.maxBufferSize = 1024ull * 1024ull * 1024ull; // request 1 GB

		if (adapter->getLimits(&limits) != wgpu::Status::Success)
		{
			throw std::runtime_error("Failed to query adapter limits");
		}

		s_UniformBufferOffsetAlignment = limits.minUniformBufferOffsetAlignment;
		s_StorageBufferOffsetAlignment = limits.minStorageBufferOffsetAlignment;

		wgpu::DeviceDescriptor deviceDesc;
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

		s_Device = std::move(adapter->requestDevice(deviceDesc));
		if(!s_Device)
		{
			throw std::runtime_error("Failed to request WGPU device");
		}

		s_Queue = std::move(s_Device->getQueue());
		if(!s_Queue)
		{
			throw std::runtime_error("Failed to get WGPU queue");
		}

		Engine::SamplerPool::Initialize();
		Engine::RenderPipelineLayouts::Initialize();
	}

	wgpu::Instance GetInstance()
	{
		return *s_Instance;
	}

	wgpu::Device GetDevice()
	{
		return *s_Device;
	}

	wgpu::Queue GetQueue()
	{
		return *s_Queue;
	}

	uint32_t GetUniformBufferOffsetAlignment()
	{
		return s_UniformBufferOffsetAlignment;
	}

	uint32_t GetStorageBufferOffsetAlignment()
	{
		return s_StorageBufferOffsetAlignment;
	}
}