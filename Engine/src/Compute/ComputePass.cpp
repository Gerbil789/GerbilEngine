#include "enginepch.h"
#include "Engine/Compute/ComputePass.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/WebGPUUtils.h"

namespace Engine
{
	namespace
	{
		wgpu::ComputePipeline m_ComputePipeline;
		wgpu::BindGroupLayout m_bindGroupLayout;
		wgpu::PipelineLayout m_pipelineLayout;

		wgpu::BindGroup m_bindGroup;

		wgpu::Buffer m_outputBuffer;
		wgpu::Buffer m_mapBuffer;
		uint64_t m_bufferSize;
	}


	ComputePass::ComputePass()
	{
		wgpu::ShaderModule computeShaderModule = LoadWGSLShader("Resources/Engine/shaders/compute.wgsl");

		// Create bind group layout
		std::vector<wgpu::BindGroupLayoutEntry> bindings(2, wgpu::Default);

		// Input buffer
		bindings[0].binding = 0;
		bindings[0].buffer.type = wgpu::BufferBindingType::ReadOnlyStorage;
		bindings[0].visibility = wgpu::ShaderStage::Compute;

		// Output buffer
		bindings[1].binding = 1;
		bindings[1].buffer.type = wgpu::BufferBindingType::Storage;
		bindings[1].visibility = wgpu::ShaderStage::Compute;

		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
		bindGroupLayoutDesc.entryCount = (uint32_t)bindings.size();
		bindGroupLayoutDesc.entries = bindings.data();
		m_bindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(bindGroupLayoutDesc);


		// Create compute pipeline layout
		wgpu::PipelineLayoutDescriptor pipelineLayoutDesc;
		pipelineLayoutDesc.bindGroupLayoutCount = 1;
		pipelineLayoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)&m_bindGroupLayout;
		m_pipelineLayout = GraphicsContext::GetDevice().createPipelineLayout(pipelineLayoutDesc);

		// Create compute pipeline;
		wgpu::ComputePipelineDescriptor computePipelineDesc = wgpu::Default;
		computePipelineDesc.compute.entryPoint = { "computeStuff", WGPU_STRLEN };
		computePipelineDesc.compute.module = computeShaderModule;
		computePipelineDesc.layout = m_pipelineLayout;
		m_ComputePipeline = GraphicsContext::GetDevice().createComputePipeline(computePipelineDesc);



		m_bufferSize = 200 * sizeof(float);

		// Create input buffer
		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.mappedAtCreation = false;
		bufferDesc.size = m_bufferSize;
		bufferDesc.usage = wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst;
		wgpu::Buffer inputBuffer = GraphicsContext::GetDevice().createBuffer(bufferDesc);

		// Add the CopySrc usage here, so that we can copy to the map buffer
		bufferDesc.usage = wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopySrc;
		m_outputBuffer = GraphicsContext::GetDevice().createBuffer(bufferDesc);

		// Create an intermediary buffer to which we copy the output and that can be
		// used for reading into the CPU memory.
		bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::MapRead;
		m_mapBuffer = GraphicsContext::GetDevice().createBuffer(bufferDesc);



		// Fill in input buffer
		std::vector<float> input(m_bufferSize / sizeof(float));
		for (int i = 0; i < input.size(); ++i) {
			input[i] = i;
		}
		GraphicsContext::GetQueue().writeBuffer(inputBuffer, 0, input.data(), m_bufferSize);


		// Create compute bind group
		std::vector<wgpu::BindGroupEntry> entries(2, wgpu::Default);

		// Input buffer
		entries[0].binding = 0;
		entries[0].buffer = inputBuffer;
		entries[0].offset = 0;
		entries[0].size = m_bufferSize;

		// Output buffer
		entries[1].binding = 1;
		entries[1].buffer = m_outputBuffer;
		entries[1].offset = 0;
		entries[1].size = m_bufferSize;

		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.layout = m_bindGroupLayout;
		bindGroupDesc.entryCount = (uint32_t)entries.size();
		bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
		m_bindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);

	}

	void ComputePass::Execute()
	{
		auto encoder = Engine::GraphicsContext::GetDevice().createCommandEncoder();

		wgpu::ComputePassDescriptor computePassDesc;
		computePassDesc.timestampWrites = nullptr;
		wgpu::ComputePassEncoder computePass = encoder.beginComputePass(computePassDesc);

		computePass.setPipeline(m_ComputePipeline);
		computePass.setBindGroup(0, m_bindGroup, 0, nullptr);

		uint32_t invocationCount = m_bufferSize / sizeof(float);
		const uint32_t workgroupSize = 32;
		uint32_t workgroupCount = (invocationCount + workgroupSize - 1) / workgroupSize;
		computePass.dispatchWorkgroups(workgroupCount, 1, 1);

		computePass.end();

		encoder.copyBufferToBuffer(m_outputBuffer, 0, m_mapBuffer, 0, m_bufferSize);

		wgpu::CommandBuffer commandBuffer = encoder.finish();
		Engine::GraphicsContext::GetQueue().submit(1, &commandBuffer);



		struct MapContext 
		{
			wgpu::Buffer buffer;
			const float* data = nullptr;
		};

		auto ctx = new MapContext();
		ctx->buffer = m_mapBuffer;

		wgpu::BufferMapCallbackInfo callbackInfo;
		callbackInfo.mode = wgpu::CallbackMode::AllowProcessEvents;
		callbackInfo.userdata1 = ctx;

		callbackInfo.callback = [](WGPUMapAsyncStatus status, WGPUStringView message, void* userdata1, void*) {
			auto* ctx = static_cast<MapContext*>(userdata1);
			if (status == wgpu::MapAsyncStatus::Success) 
			{
				ctx->data = (const float*)ctx->buffer.getConstMappedRange(0, m_bufferSize);
			}
			else 
			{
				LOG_ERROR("Map failed: {}", message.data);
			}
		};


		wgpu::FutureWaitInfo waitInfo;
		waitInfo.future = m_mapBuffer.mapAsync(wgpu::MapMode::Read, 0, m_bufferSize, callbackInfo);
		Engine::GraphicsContext::GetInstance().waitAny(1, &waitInfo, UINT64_MAX);


		for (uint32_t i = 0; i < m_bufferSize / sizeof(float); ++i)
		{
			LOG_TRACE("Output[{}] = {}", i, ctx->data[i]);
		}

		ctx->buffer.unmap();
		delete ctx;
	}
}