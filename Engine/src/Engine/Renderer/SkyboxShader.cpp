#include "enginepch.h"
#include "SkyboxShader.h"
#include "Engine/Renderer/GraphicsContext.h"
#include "Engine/Renderer/WebGPUUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Utils/File.h"

namespace Engine
{
	wgpu::ShaderModule CreateShaderModule(const std::string& source)
	{
		const char* shaderSource = source.c_str();

		wgpu::ShaderModuleDescriptor shaderDesc;
		shaderDesc.label = { "SkyboxShaderModule", WGPU_STRLEN };
		wgpu::ShaderModuleWGSLDescriptor shaderCodeDesc;
		shaderCodeDesc.chain.next = nullptr;
		shaderCodeDesc.chain.sType = wgpu::SType::ShaderSourceWGSL;
		shaderDesc.nextInChain = &shaderCodeDesc.chain;
		shaderCodeDesc.code = { shaderSource, WGPU_STRLEN };
		return GraphicsContext::GetDevice().createShaderModule(shaderDesc);
	}


	Engine::SkyboxShader::SkyboxShader(const std::filesystem::path& path)
	{
		std::string source;
		if (!Engine::ReadFile(path, source))
		{
			LOG_ERROR("Failed to read shader file. %s", path);
			return;
		}

		auto device = GraphicsContext::GetDevice();
		auto queue = GraphicsContext::GetQueue();

		wgpu::ShaderModule shaderModule = CreateShaderModule(source);

		//std::array<wgpu::VertexAttribute, 1> vertexAttribs;

		//vertexAttribs[0].format = wgpu::VertexFormat::Float32x3;
		//vertexAttribs[0].offset = 0;
		//vertexAttribs[0].shaderLocation = 0;

		//wgpu::VertexBufferLayout vertexBufferLayout;
		//vertexBufferLayout.attributeCount = 1;
		//vertexBufferLayout.attributes = vertexAttribs.data();
		//vertexBufferLayout.arrayStride = sizeof(glm::vec3);
		//vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;

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
		pipelineDesc.primitive.cullMode = wgpu::CullMode::None; //TODO: Add culling later

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

		wgpu::DepthStencilState depthStencilState{};
		depthStencilState.depthCompare = wgpu::CompareFunction::LessEqual;
		depthStencilState.depthWriteEnabled = wgpu::OptionalBool::False;
		depthStencilState.format = wgpu::TextureFormat::Depth24Plus;
		depthStencilState.stencilReadMask = 0xFFFFFFFF;
		depthStencilState.stencilWriteMask = 0xFFFFFFFF;
		pipelineDesc.depthStencil = &depthStencilState;

		pipelineDesc.multisample.count = 1;		// no multisampling
		pipelineDesc.multisample.mask = ~0u;	// all samples enabled
		pipelineDesc.multisample.alphaToCoverageEnabled = false;

		std::array<wgpu::BindGroupLayoutEntry, 2> layoutEntries;

		{
			wgpu::BindGroupLayoutEntry entry = wgpu::Default;
			entry.binding = 0;
			entry.visibility = wgpu::ShaderStage::Fragment;
			entry.sampler.type = wgpu::SamplerBindingType::Filtering;
			layoutEntries[0] = entry;
		}

		{
			wgpu::BindGroupLayoutEntry entry = wgpu::Default;
			entry.binding = 1;
			entry.visibility = wgpu::ShaderStage::Fragment;
			entry.texture.sampleType = wgpu::TextureSampleType::Float;
			entry.texture.viewDimension = wgpu::TextureViewDimension::Cube;
			entry.texture.multisampled = false;
			layoutEntries[1] = entry;
		}

		wgpu::BindGroupLayoutDescriptor desc{};
		desc.label = { "SkyboxBindGroupLayout", WGPU_STRLEN };
		desc.entryCount = 2;
		desc.entries = layoutEntries.data();

		m_BindGroupLayout = device.createBindGroupLayout(desc);

		wgpu::BindGroupLayout bindGroupLayouts[] = {
			Renderer::GetFrameBindGroupLayout(),
			m_BindGroupLayout
		};

		wgpu::PipelineLayoutDescriptor layoutDesc{};
		layoutDesc.label = { "SkyboxPipelineLayout", WGPU_STRLEN };
		layoutDesc.bindGroupLayoutCount = 2;
		layoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)&bindGroupLayouts;
		pipelineDesc.layout = device.createPipelineLayout(layoutDesc);

		m_RenderPipeline = device.createRenderPipeline(pipelineDesc);

		shaderModule.release();
	}
}