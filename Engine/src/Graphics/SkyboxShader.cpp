#include "enginepch.h"
#include "Engine/Graphics/SkyboxShader.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/WebGPUUtils.h"
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"
#include "Engine/Graphics/Renderer/RenderUniforms.h"

namespace Engine
{
	Engine::SkyboxShader::SkyboxShader(const std::filesystem::path& path)
	{
		wgpu::ShaderModule shaderModule = LoadWGSLShader(path);

		wgpu::RenderPipelineDescriptor pipelineDesc;
		pipelineDesc.label = { "SkyboxShaderPipeline", WGPU_STRLEN };

		pipelineDesc.vertex.bufferCount = 0;
		//pipelineDesc.vertex.buffers = &vertexBufferLayout;
		pipelineDesc.vertex.module = shaderModule;
		pipelineDesc.vertex.entryPoint = { "vs_main", WGPU_STRLEN};
		pipelineDesc.vertex.constantCount = 0;
		pipelineDesc.vertex.constants = nullptr;

		pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
		pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
		pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
		pipelineDesc.primitive.cullMode = wgpu::CullMode::None;

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
		fragmentState.module = shaderModule;
		fragmentState.entryPoint = { "fs_main", WGPU_STRLEN };
		fragmentState.constantCount = 0;
		fragmentState.constants = nullptr;
		fragmentState.targetCount = 1;
		fragmentState.targets = &colorTarget;
		pipelineDesc.fragment = &fragmentState;

		//wgpu::DepthStencilState depthStencilState{};
		//depthStencilState.depthCompare = wgpu::CompareFunction::LessEqual;
		//depthStencilState.depthWriteEnabled = wgpu::OptionalBool::False;
		//depthStencilState.format = wgpu::TextureFormat::Depth24Plus;
		//depthStencilState.stencilReadMask = 0xFFFFFFFF;
		//depthStencilState.stencilWriteMask = 0xFFFFFFFF;
		//pipelineDesc.depthStencil = &depthStencilState;

		pipelineDesc.multisample.count = 1;		// no multisampling
		pipelineDesc.multisample.mask = ~0u;	// all samples enabled
		pipelineDesc.multisample.alphaToCoverageEnabled = false;

		std::array<wgpu::BindGroupLayout, 2> bindGroupLayouts
		{
			RenderPipelineLayouts::GetViewLayout(),
			RenderPipelineLayouts::GetEnvironmentLayout()
		};

		wgpu::PipelineLayoutDescriptor layoutDesc;
		layoutDesc.label = { "SkyboxPipelineLayout", WGPU_STRLEN };
		layoutDesc.bindGroupLayoutCount = bindGroupLayouts.size();
		layoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)bindGroupLayouts.data();
		pipelineDesc.layout = GraphicsContext::GetDevice().createPipelineLayout(layoutDesc);

		m_RenderPipeline = GraphicsContext::GetDevice().createRenderPipeline(pipelineDesc);
	}
}