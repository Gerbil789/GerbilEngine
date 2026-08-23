#include "enginepch.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/SamplerPool.h"
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"
//#include <webgpu/webgpu_cpp_print.h>

namespace Engine::GraphicsContext
{
	namespace
	{
		static wgpu::Adapter s_Adapter;
		static wgpu::Instance s_Instance;
		static wgpu::Device s_Device;
		static wgpu::Queue s_Queue;

		static uint32_t s_UniformBufferOffsetAlignment;
		static uint32_t s_StorageBufferOffsetAlignment;

		static wgpu::TextureFormat s_SurfaceFormat;
	}

	void Initialize()
	{
		wgpu::InstanceDescriptor desc;

		std::array<wgpu::InstanceFeatureName, 1> features = { wgpu::InstanceFeatureName::TimedWaitAny };
		desc.requiredFeatureCount = features.size();
		desc.requiredFeatures = features.data();

		s_Instance = wgpu::CreateInstance(&desc);
		if (!s_Instance)
		{
			throw std::runtime_error("Failed to create WGPU instance");
		}

		wgpu::RequestAdapterOptions adapterOpts;

		s_Instance.RequestAdapter(&adapterOpts, wgpu::CallbackMode::AllowProcessEvents,
			[](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter, wgpu::StringView message)
			{
				if (status == wgpu::RequestAdapterStatus::Success)
				{
					s_Adapter = adapter;
				}
				else
				{
					LOG_ERROR("{}", std::string_view(message));
				}
			}
		);

		while (!s_Adapter)
		{
			s_Instance.ProcessEvents();
			//TODO: add timeout
		}

		{
			wgpu::AdapterInfo info;
			s_Adapter.GetInfo(&info);

			//std::stringstream ss;
			//ss << info.backendType;

			//LOG_TRACE("Dawn backend: {}", ss.str());
			LOG_TRACE("GPU: {} ({})", std::string_view(info.device), std::string_view(info.architecture));
			LOG_TRACE("Description: {}", std::string_view(info.description));
			LOG_TRACE("VendorID 0x{:X}", info.vendorID);
			LOG_TRACE("DeviceID 0x{:X}", info.deviceID);
		}

		wgpu::Limits limits;
		if (s_Adapter.GetLimits(&limits) != wgpu::Status::Success)
		{
			throw std::runtime_error("Failed to query adapter limits");
		}

		s_UniformBufferOffsetAlignment = limits.minUniformBufferOffsetAlignment;
		s_StorageBufferOffsetAlignment = limits.minStorageBufferOffsetAlignment;

		wgpu::DeviceDescriptor deviceDesc;
		deviceDesc.label = "MainDevice";
		deviceDesc.requiredFeatureCount = 0;
		deviceDesc.defaultQueue.label = "DefaultQueue";
		deviceDesc.requiredLimits = &limits;

		deviceDesc.SetDeviceLostCallback(wgpu::CallbackMode::AllowSpontaneous, [](wgpu::Device const&, wgpu::DeviceLostReason reason, wgpu::StringView message)
			{
				if (reason == wgpu::DeviceLostReason::Destroyed) return; // ignore shutdown losses (explicit destroy)

				//std::stringstream ss;
				//ss << reason;
				//LOG_ERROR("WebGPU device lost. Reason: {}, Message: {}", ss.str(), std::string_view(message));
			});

		deviceDesc.SetUncapturedErrorCallback([](wgpu::Device const&, wgpu::ErrorType type, wgpu::StringView message)
			{
				//std::stringstream ss;
				//ss << type;
				//LOG_ERROR("WebGPU Uncaptured error [type: {}]: {}", ss.str(), std::string_view(message));
			});


		s_Adapter.RequestDevice(&deviceDesc, wgpu::CallbackMode::AllowProcessEvents,
			[](wgpu::RequestDeviceStatus status, wgpu::Device device, wgpu::StringView message)
			{
				if (status == wgpu::RequestDeviceStatus::Success)
				{
					s_Device = device;
				}
				else
				{
					LOG_ERROR("{}", std::string_view(message));
				}
			}
		);

		while (!s_Device)
		{
			s_Instance.ProcessEvents();
			//TODO: add timeout
		}

		s_Queue = s_Device.GetQueue();
		if (!s_Queue)
		{
			throw std::runtime_error("Failed to get WGPU queue");
		}

		Engine::SamplerPool::Initialize();
		Engine::RenderPipelineLayouts::Initialize();
	}

	void Shutdown()
	{
		Engine::SamplerPool::Shutdown();

		if (s_Device)
		{
			s_Device.Destroy();
		}

		s_Queue = nullptr;
		s_Device = nullptr;
		s_Adapter = nullptr;
		s_Instance = nullptr;
	}

	wgpu::Adapter GetAdapter()
	{
		return s_Adapter;
	}

	wgpu::Instance GetInstance()
	{
		return s_Instance;
	}

	wgpu::Device GetDevice()
	{
		return s_Device;
	}

	wgpu::Queue GetQueue()
	{
		return s_Queue;
	}

	uint32_t GetUniformBufferOffsetAlignment()
	{
		return s_UniformBufferOffsetAlignment;
	}

	uint32_t GetStorageBufferOffsetAlignment()
	{
		return s_StorageBufferOffsetAlignment;
	}

	void SetSurfaceFormat(wgpu::TextureFormat format)
	{
		s_SurfaceFormat = format;
	}

	wgpu::TextureFormat GetSurfaceFormat()
	{
		return s_SurfaceFormat;
	}
}