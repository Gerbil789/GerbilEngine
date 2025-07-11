#include "enginepch.h"
#include "Shader.h"
#include "Engine/Core/Application.h"
#include "Engine/Utils/File.h"
#include "Engine/Renderer/RenderUtils.h"

namespace Engine 
{
	Shader::Shader(const std::filesystem::path& path) : Asset(path)
	{
		auto device = Application::Get().GetGraphicsContext()->GetDevice();
		auto queue = Application::Get().GetGraphicsContext()->GetQueue();

		wgpu::ShaderModule shaderModule = LoadShader("resources/shaders/testshader.wgsl");

		std::vector<wgpu::VertexAttribute> vertexAttribs(3);

		// Describe the position attribute
		vertexAttribs[0].shaderLocation = 0; // @location(0)
		vertexAttribs[0].format = wgpu::VertexFormat::Float32x3;
		vertexAttribs[0].offset = 0;

		// Describe the normal attribute
		vertexAttribs[1].shaderLocation = 1; // @location(1)
		vertexAttribs[1].format = wgpu::VertexFormat::Float32x3;
		vertexAttribs[1].offset = 3 * sizeof(float);

		// Describe the uv attribute
		vertexAttribs[2].shaderLocation = 2; // @location(2)
		vertexAttribs[2].format = wgpu::VertexFormat::Float32x2;
		vertexAttribs[2].offset = 6 * sizeof(float);

		wgpu::VertexBufferLayout vertexBufferLayout;

		vertexBufferLayout.attributeCount = static_cast<uint32_t>(vertexAttribs.size());
		vertexBufferLayout.attributes = vertexAttribs.data();

		vertexBufferLayout.arrayStride = 8 * sizeof(float);
		vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;

		wgpu::RenderPipelineDescriptor pipelineDesc;

		pipelineDesc.vertex.bufferCount = 1;
		pipelineDesc.vertex.buffers = &vertexBufferLayout;

		pipelineDesc.vertex.module = shaderModule;
		pipelineDesc.vertex.entryPoint = { "vs_main", strlen("vs_main") };
		pipelineDesc.vertex.constantCount = 0;
		pipelineDesc.vertex.constants = nullptr;

		pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;	// Each sequence of 3 vertices is considered as a triangle
		pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;		// When not specified, vertices are considered sequentially.
		pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
		pipelineDesc.primitive.cullMode = wgpu::CullMode::None; //TODO: Add culling later

		wgpu::FragmentState fragmentState;
		fragmentState.module = shaderModule;
		fragmentState.entryPoint = { "fs_main", strlen("fs_main") };
		fragmentState.constantCount = 0;
		fragmentState.constants = nullptr;

		wgpu::BlendState blendState;
		blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
		blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
		blendState.color.operation = wgpu::BlendOperation::Add;
		blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
		blendState.alpha.dstFactor = wgpu::BlendFactor::One;
		blendState.alpha.operation = wgpu::BlendOperation::Add;

		wgpu::ColorTargetState colorTarget;
		colorTarget.format = WGPUTextureFormat_RGBA8Unorm;
		colorTarget.blend = &blendState;
		colorTarget.writeMask = wgpu::ColorWriteMask::All;

		fragmentState.targetCount = 1;
		fragmentState.targets = &colorTarget;
		pipelineDesc.fragment = &fragmentState;

		pipelineDesc.depthStencil = nullptr;
		pipelineDesc.multisample.count = 1;
		pipelineDesc.multisample.mask = ~0u; // Default value for the mask, meaning "all bits on"
		pipelineDesc.multisample.alphaToCoverageEnabled = false;

		wgpu::BindGroupLayout bindGroupLayouts[] = {
			RenderUtils::s_ModelBindGroupLayout,
			RenderUtils::s_FrameBindGroupLayout
		};

		wgpu::PipelineLayoutDescriptor layoutDesc{};
		layoutDesc.label = { "ShaderPipelineLayout", WGPU_STRLEN };
		layoutDesc.bindGroupLayoutCount = 2;
		layoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)&bindGroupLayouts;
		wgpu::PipelineLayout layout = device.createPipelineLayout(layoutDesc);
		pipelineDesc.layout = layout;

		m_RenderPipeline = device.createRenderPipeline(pipelineDesc);
		shaderModule.release();
	}

	wgpu::ShaderModule Shader::LoadShader(const std::filesystem::path& path)
	{
		auto device = Application::Get().GetGraphicsContext()->GetDevice();

		std::string content;
		if (!Engine::ReadFile(path, content))
		{
			LOG_ERROR("Failed to load shader. %s", path.string());
			return nullptr;
		}

		const char* shaderSource = content.c_str();

		wgpu::ShaderModuleDescriptor shaderDesc;
		wgpu::ShaderModuleWGSLDescriptor shaderCodeDesc;
		shaderCodeDesc.chain.next = nullptr;
		shaderCodeDesc.chain.sType = wgpu::SType::ShaderSourceWGSL;
		shaderDesc.nextInChain = &shaderCodeDesc.chain;
		shaderCodeDesc.code = { shaderSource, strlen(shaderSource) };

		return device.createShaderModule(shaderDesc);
	}
}