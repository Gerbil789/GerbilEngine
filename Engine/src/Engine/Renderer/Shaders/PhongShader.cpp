#include "enginepch.h"
#include "PhongShader.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/GraphicsContext.h"

namespace Engine
{
	PhongShader::PhongShader()
	{
		AddParameter("Color", GroupID::Material, 0, ShaderParamType::Vec4, sizeof(glm::vec4));
		AddParameter("Sampler", GroupID::Material, 1, ShaderParamType::Sampler, 0);
		AddParameter("AlbedoTexture", GroupID::Material, 2, ShaderParamType::Texture2D, 0);

		auto device = GraphicsContext::GetDevice();
		auto queue = GraphicsContext::GetQueue();

		wgpu::ShaderModule shaderModule = LoadShader("Engine/resources/shaders/phong.wgsl");

		std::vector<wgpu::VertexAttribute> vertexAttribs(3);

		// Position
		vertexAttribs[0].shaderLocation = 0; // @location(0)
		vertexAttribs[0].format = wgpu::VertexFormat::Float32x3;
		vertexAttribs[0].offset = 0;

		// Normal
		vertexAttribs[1].shaderLocation = 1; // @location(1)
		vertexAttribs[1].format = wgpu::VertexFormat::Float32x3;
		vertexAttribs[1].offset = 3 * sizeof(float);

		// UV
		vertexAttribs[2].shaderLocation = 2; // @location(2)
		vertexAttribs[2].format = wgpu::VertexFormat::Float32x2;
		vertexAttribs[2].offset = 6 * sizeof(float);

		wgpu::VertexBufferLayout vertexBufferLayout;

		vertexBufferLayout.attributeCount = static_cast<uint32_t>(vertexAttribs.size());
		vertexBufferLayout.attributes = vertexAttribs.data();
		vertexBufferLayout.arrayStride = 8 * sizeof(float);
		vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;


		wgpu::RenderPipelineDescriptor pipelineDesc;
		pipelineDesc.label = { "ShaderPipeline", WGPU_STRLEN };

		pipelineDesc.vertex.bufferCount = 1;
		pipelineDesc.vertex.buffers = &vertexBufferLayout;
		pipelineDesc.vertex.module = shaderModule;
		pipelineDesc.vertex.entryPoint = { "vs_main", WGPU_STRLEN };
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
		colorTarget.format = WGPUTextureFormat_RGBA8Unorm;
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
		depthStencilState.depthCompare = wgpu::CompareFunction::Less;
		depthStencilState.depthWriteEnabled = wgpu::OptionalBool::True;
		depthStencilState.format = wgpu::TextureFormat::Depth24Plus;
		depthStencilState.stencilReadMask = 0xFFFFFFFF;
		depthStencilState.stencilWriteMask = 0xFFFFFFFF;
		pipelineDesc.depthStencil = &depthStencilState;

		pipelineDesc.multisample.count = 1;
		pipelineDesc.multisample.mask = ~0u; // Default value for the mask, meaning "all bits on"
		pipelineDesc.multisample.alphaToCoverageEnabled = false;

		m_MaterialBindGroupLayout = CreateMaterialBindGroupLayout();

		wgpu::BindGroupLayout bindGroupLayouts[] = {
			Renderer::GetFrameBindGroupLayout(),
			Renderer::GetModelBindGroupLayout(),
			m_MaterialBindGroupLayout
		};

		wgpu::PipelineLayoutDescriptor layoutDesc{};
		layoutDesc.label = { "ShaderPipelineLayout", WGPU_STRLEN };
		layoutDesc.bindGroupLayoutCount = 3;
		layoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)&bindGroupLayouts;
		pipelineDesc.layout = device.createPipelineLayout(layoutDesc);

		m_RenderPipeline = device.createRenderPipeline(pipelineDesc);
		shaderModule.release();
	}
}