#include "enginepch.h"
#include "Engine/Graphics/SkyboxShader.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/WebGPUUtils.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"

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

		std::array<wgpu::BindGroupLayoutEntry, 2> layoutEntries;

		{
			wgpu::BindGroupLayoutEntry entry = wgpu::Default;
			entry.binding = 0;
			entry.visibility = wgpu::ShaderStage::Fragment;
			entry.sampler.type = wgpu::SamplerBindingType::NonFiltering;
			layoutEntries[0] = entry;
		}

		{
			wgpu::BindGroupLayoutEntry entry = wgpu::Default;
			entry.binding = 1;
			entry.visibility = wgpu::ShaderStage::Fragment;
			entry.texture.sampleType = wgpu::TextureSampleType::UnfilterableFloat;
			entry.texture.viewDimension = wgpu::TextureViewDimension::Cube;
			entry.texture.multisampled = false;
			layoutEntries[1] = entry;
		}

		wgpu::BindGroupLayoutDescriptor desc;
		desc.label = { "SkyboxBindGroupLayout", WGPU_STRLEN };
		desc.entryCount = layoutEntries.size();
		desc.entries = layoutEntries.data();

		m_BindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(desc);

		std::array<wgpu::BindGroupLayout, 2> bindGroupLayouts
		{
			RenderGlobals::GetFrameLayout(),
			m_BindGroupLayout
		};

		wgpu::PipelineLayoutDescriptor layoutDesc;
		layoutDesc.label = { "SkyboxPipelineLayout", WGPU_STRLEN };
		layoutDesc.bindGroupLayoutCount = bindGroupLayouts.size();
		layoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)&bindGroupLayouts;
		pipelineDesc.layout = GraphicsContext::GetDevice().createPipelineLayout(layoutDesc);

		m_RenderPipeline = GraphicsContext::GetDevice().createRenderPipeline(pipelineDesc);
	}
}