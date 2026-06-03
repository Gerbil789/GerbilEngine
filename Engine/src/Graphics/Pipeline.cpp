#include "enginepch.h"
#include "Engine/Graphics/Pipeline.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"
#include "Engine/Graphics/WebGPUUtils.h"
#include "Engine/Asset/AssetManager.h"
#include <numeric>

namespace Engine
{
	wgpu::RenderPipeline PipelineCache::GetOrCreatePipeline(const PipelineSpecification& specification)
	{
		size_t configHash = specification.Hash();
		auto it = s_PipelineCache.find(configHash);
		if (it != s_PipelineCache.end())
		{
			return it->second;
		}

		const Shader& shader = Engine::AssetManager::GetAsset<Shader>(specification.shaderId);

		auto vertexAttributes = shader.GetSpecification().vertexAttributes;

		const uint64_t offset = std::accumulate(vertexAttributes.begin(), vertexAttributes.end(), 0ull,
			[](uint64_t sum, const wgpu::VertexAttribute& attr)
			{
				return sum + GetVertexFormatSize(attr.format);
			}
		);

		wgpu::VertexBufferLayout vertexBufferLayout;
		vertexBufferLayout.attributeCount = vertexAttributes.size();
		vertexBufferLayout.attributes = vertexAttributes.data();
		vertexBufferLayout.arrayStride = offset;
		vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;

		wgpu::RenderPipelineDescriptor pipelineDesc;
		//std::string pipelineLabel = std::format("{} Shader Pipeline", name);
		pipelineDesc.label = { "Shader Pipeline", WGPU_STRLEN};

		pipelineDesc.vertex.bufferCount = 1;
		pipelineDesc.vertex.buffers = &vertexBufferLayout;
		pipelineDesc.vertex.module = shader.GetShaderModule();
		pipelineDesc.vertex.entryPoint = { "vs_main", WGPU_STRLEN};
		pipelineDesc.vertex.constantCount = 0;
		pipelineDesc.vertex.constants = nullptr;

		pipelineDesc.primitive.topology = specification.topology;
		pipelineDesc.primitive.frontFace = wgpu::FrontFace::CW;
		pipelineDesc.primitive.cullMode = specification.cullMode;

		wgpu::BlendState blendState;
		blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
		blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
		blendState.color.operation = wgpu::BlendOperation::Add;
		blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
		blendState.alpha.dstFactor = wgpu::BlendFactor::One;
		blendState.alpha.operation = wgpu::BlendOperation::Add;

		wgpu::ColorTargetState colorTarget;
		colorTarget.format = wgpu::TextureFormat::RGBA8Unorm;
		colorTarget.blend = &blendState;
		colorTarget.writeMask = wgpu::ColorWriteMask::All;

		wgpu::FragmentState fragmentState;
		fragmentState.module = shader.GetShaderModule();
		fragmentState.entryPoint = { "fs_main", WGPU_STRLEN };
		fragmentState.constantCount = 0;
		fragmentState.constants = nullptr;
		fragmentState.targetCount = 1;
		fragmentState.targets = &colorTarget;
		pipelineDesc.fragment = &fragmentState;

		wgpu::DepthStencilState depthStencilState {};
		depthStencilState.depthCompare = wgpu::CompareFunction::Less;
		depthStencilState.depthWriteEnabled = wgpu::OptionalBool::True;
		depthStencilState.format = wgpu::TextureFormat::Depth24Plus;
		depthStencilState.stencilReadMask = 0xFFFFFFFF;
		depthStencilState.stencilWriteMask = 0xFFFFFFFF;

		depthStencilState.depthBias = 2;
		depthStencilState.depthBiasSlopeScale = 2.0f;
		depthStencilState.depthBiasClamp = 0.0f;

		pipelineDesc.depthStencil = &depthStencilState;

		pipelineDesc.multisample.count = 1;		// no multisampling
		pipelineDesc.multisample.mask = ~0u;	// all samples enabled
		pipelineDesc.multisample.alphaToCoverageEnabled = false;


		std::array<wgpu::BindGroupLayout, 4> bindGroupLayouts
		{
			RenderPipelineLayouts::GetViewLayout(),
			RenderPipelineLayouts::GetEnvironmentLayout(),
			shader.GetMaterialBindGroupLayout(),
			RenderPipelineLayouts::GetModelLayout()
		};

		wgpu::PipelineLayoutDescriptor layoutDesc;
		//std::string label = std::format("{} Shader Pipeline Layout", name);
		std::string label = "Shader Pipeline Layout";
		layoutDesc.label = { label.c_str(), WGPU_STRLEN};
		layoutDesc.bindGroupLayoutCount = bindGroupLayouts.size();
		layoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)bindGroupLayouts.data();
		pipelineDesc.layout = GraphicsContext::GetDevice().createPipelineLayout(layoutDesc);

		wgpu::RenderPipeline pipeline = GraphicsContext::GetDevice().createRenderPipeline(pipelineDesc);

		s_PipelineCache[configHash] = pipeline;
		return pipeline;
		
	}

}