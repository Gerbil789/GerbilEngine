#include "enginepch.h"
#include "Engine/Graphics/Pipeline.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"
#include "Engine/Graphics/Utility.h"
#include "Engine/Asset/AssetManager.h"
#include <numeric>

namespace Engine
{
	wgpu::RenderPipeline PipelineCache::GetPipeline(const PipelineSpecification& specification)
	{
		auto it = s_PipelineCache.find(specification);
		if (it != s_PipelineCache.end())
		{
			return it->second;
		}

		const ShaderAsset& shader = Engine::AssetManager::GetAsset<ShaderAsset>(specification.shader);

		wgpu::RenderPipelineDescriptor pipelineDesc;
		pipelineDesc.label = "Shader Pipeline";


		const std::vector<wgpu::VertexAttribute>& vertexAttributes = shader.GetSpecification().vertexAttributes;

		wgpu::VertexBufferLayout vertexBufferLayout;
		if(!vertexAttributes.empty())
		{
			uint64_t stride = std::accumulate(vertexAttributes.begin(), vertexAttributes.end(), 0ull, [](uint64_t sum, const wgpu::VertexAttribute& attr) { return sum + GetVertexFormatSize(attr.format); });

			vertexBufferLayout.attributeCount = vertexAttributes.size();
			vertexBufferLayout.attributes = vertexAttributes.data();
			vertexBufferLayout.arrayStride = stride;
			vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;

			pipelineDesc.vertex.bufferCount = 1;
			pipelineDesc.vertex.buffers = &vertexBufferLayout;
		}
		else
		{
			pipelineDesc.vertex.bufferCount = 0;
			pipelineDesc.vertex.buffers = nullptr;
		}

		pipelineDesc.vertex.module = shader.GetShaderModule();
		pipelineDesc.vertex.entryPoint = "vs_main";

		pipelineDesc.primitive.topology = specification.topology;
		pipelineDesc.primitive.frontFace = specification.frontFace;
		pipelineDesc.primitive.cullMode = specification.cullMode;

		wgpu::BlendState blendState;
		blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
		blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
		blendState.color.operation = wgpu::BlendOperation::Add;
		blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
		blendState.alpha.dstFactor = wgpu::BlendFactor::One;
		blendState.alpha.operation = wgpu::BlendOperation::Add;

		wgpu::ColorTargetState colorTarget;
		colorTarget.format = GraphicsContext::GetSurfaceFormat();
		colorTarget.blend = &blendState;
		colorTarget.writeMask = wgpu::ColorWriteMask::All;

		wgpu::FragmentState fragmentState;
		fragmentState.module = shader.GetShaderModule();
		fragmentState.entryPoint = "fs_main";
		fragmentState.constantCount = 0;
		fragmentState.constants = nullptr;
		fragmentState.targetCount = 1;
		fragmentState.targets = &colorTarget;
		pipelineDesc.fragment = &fragmentState;

		wgpu::DepthStencilState depthStencilState;
		if(specification.depthWrite)
		{
			depthStencilState.depthCompare = specification.depthCompare;
			depthStencilState.depthWriteEnabled = specification.depthWrite ? wgpu::OptionalBool::True : wgpu::OptionalBool::False;
			depthStencilState.format = specification.depthFormat;
			depthStencilState.stencilReadMask = 0xFFFFFFFF;
			depthStencilState.stencilWriteMask = 0xFFFFFFFF;
			depthStencilState.depthBias = 0;
			depthStencilState.depthBiasSlopeScale = 0.0f;
			depthStencilState.depthBiasClamp = 0.0f;

			pipelineDesc.depthStencil = &depthStencilState;
		}

		else
		{
			pipelineDesc.depthStencil = nullptr; // Disable depth testing entirely
		}

		pipelineDesc.multisample.count = 1;
		pipelineDesc.multisample.mask = ~0u;


		std::vector<wgpu::BindGroupLayout> bindGroupLayouts;

		if (!specification.layoutOverrides.empty())
		{
			bindGroupLayouts = specification.layoutOverrides;
		}
		else
		{
			bindGroupLayouts =
			{
					RenderPipelineLayouts::GetViewLayout(),
					RenderPipelineLayouts::GetEnvironmentLayout(),
					shader.GetMaterialBindGroupLayout(),
					RenderPipelineLayouts::GetModelLayout()
			};
		}


		wgpu::PipelineLayoutDescriptor layoutDesc;
		layoutDesc.label = "Shader Pipeline Layout";
		layoutDesc.bindGroupLayoutCount = bindGroupLayouts.size();
		layoutDesc.bindGroupLayouts = bindGroupLayouts.data();
		pipelineDesc.layout = GraphicsContext::GetDevice().CreatePipelineLayout(&layoutDesc);

		wgpu::RenderPipeline pipeline = GraphicsContext::GetDevice().CreateRenderPipeline(&pipelineDesc);

		s_PipelineCache[specification] = pipeline;
		return pipeline;
	}

}