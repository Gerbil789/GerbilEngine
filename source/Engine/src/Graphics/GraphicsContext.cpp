#include "enginepch.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/SamplerPool.h"
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"
#include "Engine/Core/Log.h"
#include <webgpu/webgpu_cpp_print.h>

namespace engine::GraphicsContext
{
	namespace
	{
		static wgpu::Adapter s_adapter;
		static wgpu::Instance s_instance;
		static wgpu::Device s_device;
		static wgpu::Queue s_queue;

		static uint32_t s_uniformBufferOffsetAlignment;
		static uint32_t s_storageBufferOffsetAlignment;

		static wgpu::TextureFormat s_surfaceFormat;
	}

	void Initialize()
	{
		wgpu::InstanceDescriptor desc;

		std::array<wgpu::InstanceFeatureName, 1> features = { wgpu::InstanceFeatureName::TimedWaitAny };
		desc.requiredFeatureCount = features.size();
		desc.requiredFeatures = features.data();

		s_instance = wgpu::CreateInstance(&desc);
		if (!s_instance)
		{
			throw std::runtime_error("Failed to create WGPU instance");
		}

		wgpu::RequestAdapterOptions adapterOpts;

		s_instance.RequestAdapter(&adapterOpts, wgpu::CallbackMode::AllowProcessEvents,
			[](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter, wgpu::StringView message)
			{
				if (status == wgpu::RequestAdapterStatus::Success)
				{
					s_adapter = adapter;
				}
				else
				{
					LOG_ERROR("{}", std::string_view(message));
				}
			}
		);

		while (!s_adapter)
		{
			s_instance.ProcessEvents();
			//TODO: add timeout
		}

		{
			wgpu::AdapterInfo info;
			s_adapter.GetInfo(&info);

			std::stringstream ss;
			ss << info.backendType;

			LOG_TRACE("Dawn backend: {}", ss.str());
			LOG_TRACE("GPU: {} ({})", std::string_view(info.device), std::string_view(info.architecture));
			LOG_TRACE("Description: {}", std::string_view(info.description));
			LOG_TRACE("VendorID 0x{:X}", info.vendorID);
			LOG_TRACE("DeviceID 0x{:X}", info.deviceID);
		}

		wgpu::Limits limits;
		if (s_adapter.GetLimits(&limits) != wgpu::Status::Success)
		{
			throw std::runtime_error("Failed to query adapter limits");
		}

		s_uniformBufferOffsetAlignment = limits.minUniformBufferOffsetAlignment;
		s_storageBufferOffsetAlignment = limits.minStorageBufferOffsetAlignment;

		wgpu::DeviceDescriptor deviceDesc;
		deviceDesc.label = "MainDevice";
		deviceDesc.requiredFeatureCount = 0;
		deviceDesc.defaultQueue.label = "DefaultQueue";
		deviceDesc.requiredLimits = &limits;

		deviceDesc.SetDeviceLostCallback(wgpu::CallbackMode::AllowSpontaneous, [](wgpu::Device const&, wgpu::DeviceLostReason reason, wgpu::StringView message)
			{
				if (reason == wgpu::DeviceLostReason::Destroyed) return; // ignore shutdown losses (explicit destroy)

				std::stringstream ss;
				ss << reason;
				LOG_ERROR("WebGPU device lost. Reason: {}, Message: {}", ss.str(), std::string_view(message));
			});

		deviceDesc.SetUncapturedErrorCallback([](wgpu::Device const&, wgpu::ErrorType type, wgpu::StringView message)
			{
				std::stringstream ss;
				ss << type;
				LOG_ERROR("WebGPU Uncaptured error [type: {}]: {}", ss.str(), std::string_view(message));
			});


		s_adapter.RequestDevice(&deviceDesc, wgpu::CallbackMode::AllowProcessEvents,
			[](wgpu::RequestDeviceStatus status, wgpu::Device device, wgpu::StringView message)
			{
				if (status == wgpu::RequestDeviceStatus::Success)
				{
					s_device = device;
				}
				else
				{
					LOG_ERROR("{}", std::string_view(message));
				}
			}
		);

		while (!s_device)
		{
			s_instance.ProcessEvents();
			//TODO: add timeout
		}

		s_queue = s_device.GetQueue();
		if (!s_queue)
		{
			throw std::runtime_error("Failed to get WGPU queue");
		}

		engine::SamplerPool::Initialize();
		engine::RenderPipelineLayouts::Initialize();
	}

	void Shutdown()
	{
		engine::SamplerPool::Shutdown();

		if (s_device)
		{
			s_device.Destroy();
		}

		s_queue = nullptr;
		s_device = nullptr;
		s_adapter = nullptr;
		s_instance = nullptr;
	}

	wgpu::Adapter GetAdapter()
	{
		return s_adapter;
	}

	wgpu::Instance GetInstance()
	{
		return s_instance;
	}

	wgpu::Device GetDevice()
	{
		return s_device;
	}

	wgpu::Queue GetQueue()
	{
		return s_queue;
	}

	uint32_t GetUniformBufferOffsetAlignment()
	{
		return s_uniformBufferOffsetAlignment;
	}

	uint32_t GetStorageBufferOffsetAlignment()
	{
		return s_storageBufferOffsetAlignment;
	}

	void SetSurfaceFormat(wgpu::TextureFormat format)
	{
		s_surfaceFormat = format;
	}

	wgpu::TextureFormat GetSurfaceFormat()
	{
		return s_surfaceFormat;
	}
}