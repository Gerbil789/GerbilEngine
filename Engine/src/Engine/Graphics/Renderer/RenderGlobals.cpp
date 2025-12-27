#include "enginepch.h"
#include "RenderGlobals.h"
#include "Engine/Graphics/GraphicsContext.h"

namespace Engine::RenderGlobals
{
	// Model bind group
	static wgpu::BindGroupLayout s_ModelBindGroupLayout = nullptr;
	static wgpu::BindGroup s_ModelBindGroup = nullptr;
	static wgpu::Buffer s_ModelUniformBuffer = nullptr;
	static uint32_t s_ModelUniformStride = 0;

	// Frame bind group
	static wgpu::BindGroupLayout s_FrameBindGroupLayout = nullptr;
	static wgpu::BindGroup s_FrameBindGroup = nullptr;
	static wgpu::Buffer s_FrameUniformBuffer = nullptr;

	struct alignas(16) ModelUniforms
	{
		glm::mat4 modelMatrix;
	};
	static_assert(sizeof(ModelUniforms) % 16 == 0);

	static uint32_t CeilToNextMultiple(uint32_t value, uint32_t step)
	{
		uint32_t divide_and_ceil = value / step + (value % step == 0 ? 0 : 1);
		return step * divide_and_ceil;
	}


	void Initialize()
	{
		wgpu::Device device = GraphicsContext::GetDevice();
		wgpu::Queue queue = GraphicsContext::GetQueue();

		// Model 
		{
			wgpu::Limits limits;
			device.getLimits(&limits);
			s_ModelUniformStride = CeilToNextMultiple((uint32_t)sizeof(ModelUniforms), (uint32_t)limits.minUniformBufferOffsetAlignment);

			// stride is probably 256 on most hardware, but could be different on some

			const size_t BufferSize = 1024 * s_ModelUniformStride; // max 1024 unique transforms per frame

			wgpu::BindGroupLayoutEntry bindGroupLayoutEntry = wgpu::Default;
			bindGroupLayoutEntry.binding = 0;
			bindGroupLayoutEntry.visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
			bindGroupLayoutEntry.buffer.hasDynamicOffset = true;
			bindGroupLayoutEntry.buffer.type = wgpu::BufferBindingType::Uniform;
			bindGroupLayoutEntry.buffer.minBindingSize = sizeof(ModelUniforms);

			wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc{};
			bindGroupLayoutDesc.label = { "ModelBindGroupLayout", WGPU_STRLEN };
			bindGroupLayoutDesc.entryCount = 1;
			bindGroupLayoutDesc.entries = &bindGroupLayoutEntry;
			s_ModelBindGroupLayout = device.createBindGroupLayout(bindGroupLayoutDesc);

			wgpu::BufferDescriptor bufferDesc{};
			bufferDesc.label = { "ModelUniformBuffer", WGPU_STRLEN };
			bufferDesc.size = BufferSize;
			bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
			s_ModelUniformBuffer = device.createBuffer(bufferDesc);

			wgpu::BindGroupEntry bindGroupEntry{};
			bindGroupEntry.binding = 0;
			bindGroupEntry.buffer = s_ModelUniformBuffer;
			bindGroupEntry.offset = 0;
			bindGroupEntry.size = sizeof(ModelUniforms);

			wgpu::BindGroupDescriptor bindGroupDesc{};
			bindGroupDesc.label = { "ModelBindGroup", WGPU_STRLEN };
			bindGroupDesc.layout = s_ModelBindGroupLayout;
			bindGroupDesc.entryCount = 1;
			bindGroupDesc.entries = &bindGroupEntry;
			s_ModelBindGroup = device.createBindGroup(bindGroupDesc);
		}

		// Frame 
		{
			wgpu::BindGroupLayoutEntry bindGroupLayoutEntry = wgpu::Default;
			bindGroupLayoutEntry.binding = 0;
			bindGroupLayoutEntry.visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
			bindGroupLayoutEntry.buffer.type = wgpu::BufferBindingType::Uniform;
			bindGroupLayoutEntry.buffer.minBindingSize = sizeof(FrameUniforms);

			wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc{};
			bindGroupLayoutDesc.label = { "FrameBindGroupLayout", WGPU_STRLEN };
			bindGroupLayoutDesc.entryCount = 1;
			bindGroupLayoutDesc.entries = &bindGroupLayoutEntry;
			s_FrameBindGroupLayout = device.createBindGroupLayout(bindGroupLayoutDesc);

			wgpu::BufferDescriptor bufferDesc{};
			bufferDesc.label = { "FrameUniformBuffer", WGPU_STRLEN };
			bufferDesc.size = sizeof(FrameUniforms);
			bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;

			s_FrameUniformBuffer = device.createBuffer(bufferDesc);

			wgpu::BindGroupEntry bindGroupEntry{};
			bindGroupEntry.binding = 0;
			bindGroupEntry.buffer = s_FrameUniformBuffer;
			bindGroupEntry.offset = 0;
			bindGroupEntry.size = sizeof(FrameUniforms);

			wgpu::BindGroupDescriptor bindGroupDesc{};
			bindGroupDesc.label = { "FrameBindGroup", WGPU_STRLEN };
			bindGroupDesc.layout = s_FrameBindGroupLayout;
			bindGroupDesc.entryCount = 1;
			bindGroupDesc.entries = &bindGroupEntry;
			s_FrameBindGroup = device.createBindGroup(bindGroupDesc);
		}
	}

	wgpu::BindGroupLayout GetFrameLayout()
	{
		return s_FrameBindGroupLayout;

	}

	wgpu::BindGroup GetFrameBindGroup()
	{
		return s_FrameBindGroup;
	}

	wgpu::Buffer GetFrameUniformBuffer()
	{
		return s_FrameUniformBuffer;
	}

	wgpu::BindGroupLayout GetModelLayout()
	{
		return s_ModelBindGroupLayout;
	}

	wgpu::BindGroup GetModelBindGroup()
	{
		return s_ModelBindGroup;
	}

	wgpu::Buffer GetModelUniformBuffer()
	{
		return s_ModelUniformBuffer;
	}

	uint32_t GetModelUniformStride()
	{
		return s_ModelUniformStride;
	}
}