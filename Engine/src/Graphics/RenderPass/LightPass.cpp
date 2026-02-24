#include "enginepch.h"
#include "Engine/Graphics/RenderPass/LightPass.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"
#include "Engine/Utility/File.h"

namespace Engine
{
	struct alignas(16) Light 
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		float range = 50.0f;
		glm::vec3 color = { 1.0f, 1.0f, 1.0f };
		float intensity = 1.0f;
	};
	static_assert(sizeof(Light) % 16 == 0);


	const uint32_t max_lights = 32;

	struct alignas(16) LightUniforms 
	{
		uint32_t count;
		std::array<Light, max_lights> lights;
	};
	static_assert(sizeof(LightUniforms) % 16 == 0);


	wgpu::RenderPipeline m_LightPipeline;

	wgpu::BindGroup s_LightBindGroup;
	wgpu::BindGroupLayout s_LightBindGroupLayout;
	wgpu::Buffer s_LightUniformBuffer;

	LightPass::LightPass()
	{
		std::string content;
		if (!Engine::ReadFile("Resources/Engine/shaders/phong.wgsl", content))
		{
			throw std::runtime_error("Failed to load phong shader");
		}

		wgpu::ShaderSourceWGSL shaderCodeDesc;
		shaderCodeDesc.chain.next = nullptr;
		shaderCodeDesc.chain.sType = wgpu::SType::ShaderSourceWGSL;
		shaderCodeDesc.code = { content.c_str(), WGPU_STRLEN };

		wgpu::ShaderModuleDescriptor shaderDesc{};
		shaderDesc.nextInChain = &shaderCodeDesc.chain;

		wgpu::ShaderModule shaderModule = GraphicsContext::GetDevice().createShaderModule(shaderDesc);
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
		pipelineDesc.label = { "LightShaderPipeline", WGPU_STRLEN };

		pipelineDesc.vertex.bufferCount = 1;
		pipelineDesc.vertex.buffers = &vertexBufferLayout;
		pipelineDesc.vertex.module = shaderModule;
		pipelineDesc.vertex.entryPoint = { "vs_main", WGPU_STRLEN };
		pipelineDesc.vertex.constantCount = 0;
		pipelineDesc.vertex.constants = nullptr;

		pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
		pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
		pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
		pipelineDesc.primitive.cullMode = wgpu::CullMode::None;


		wgpu::BlendState blend{};
		blend.color.operation = wgpu::BlendOperation::Add;
		blend.color.srcFactor = wgpu::BlendFactor::One;
		blend.color.dstFactor = wgpu::BlendFactor::One;
		blend.alpha.operation = wgpu::BlendOperation::Add;
		blend.alpha.srcFactor = wgpu::BlendFactor::One;
		blend.alpha.dstFactor = wgpu::BlendFactor::One;

		wgpu::ColorTargetState colorTarget;
		colorTarget.format = wgpu::TextureFormat::RGBA8Unorm;
		colorTarget.writeMask = wgpu::ColorWriteMask::All;
		colorTarget.blend = &blend;

		wgpu::DepthStencilState depthStencil{};
		depthStencil.format = wgpu::TextureFormat::Depth24Plus;
		depthStencil.depthWriteEnabled = wgpu::OptionalBool::False;
		depthStencil.depthCompare = wgpu::CompareFunction::Equal;
		depthStencil.stencilFront = {};
		depthStencil.stencilBack = {};
		depthStencil.stencilReadMask = 0;
		depthStencil.stencilWriteMask = 0;

		wgpu::FragmentState fragmentState;
		fragmentState.module = shaderModule;
		fragmentState.entryPoint = { "fs_main", WGPU_STRLEN };
		fragmentState.constantCount = 0;
		fragmentState.constants = nullptr;
		fragmentState.targetCount = 1;
		fragmentState.targets = &colorTarget;
		pipelineDesc.depthStencil = &depthStencil;
		pipelineDesc.fragment = &fragmentState;

		pipelineDesc.multisample.count = 1;
		pipelineDesc.multisample.mask = ~0u;
		pipelineDesc.multisample.alphaToCoverageEnabled = false;






		// Light
		{
			wgpu::BindGroupLayoutEntry bindGroupLayoutEntry = wgpu::Default;
			bindGroupLayoutEntry.binding = 0;
			bindGroupLayoutEntry.visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
			bindGroupLayoutEntry.buffer.type = wgpu::BufferBindingType::Uniform;
			bindGroupLayoutEntry.buffer.minBindingSize = sizeof(LightUniforms);

			wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc{};
			bindGroupLayoutDesc.label = { "LightBindGroupLayout", WGPU_STRLEN };
			bindGroupLayoutDesc.entryCount = 1;
			bindGroupLayoutDesc.entries = &bindGroupLayoutEntry;
			s_LightBindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(bindGroupLayoutDesc);

			wgpu::BufferDescriptor bufferDesc{};
			bufferDesc.label = { "LightUniformBuffer", WGPU_STRLEN };
			bufferDesc.size = sizeof(LightUniforms);
			bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;

			s_LightUniformBuffer = GraphicsContext::GetDevice().createBuffer(bufferDesc);

			wgpu::BindGroupEntry bindGroupEntry{};
			bindGroupEntry.binding = 0;
			bindGroupEntry.buffer = s_LightUniformBuffer;
			bindGroupEntry.offset = 0;
			bindGroupEntry.size = sizeof(LightUniforms);

			wgpu::BindGroupDescriptor bindGroupDesc{};
			bindGroupDesc.label = { "LightBindGroup", WGPU_STRLEN };
			bindGroupDesc.layout = s_LightBindGroupLayout;
			bindGroupDesc.entryCount = 1;
			bindGroupDesc.entries = &bindGroupEntry;
			s_LightBindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);
		}

		wgpu::BindGroupLayout bindGroupLayouts[] = {
			RenderGlobals::GetFrameLayout(),
			RenderGlobals::GetModelLayout(),
			s_LightBindGroupLayout
		};

		wgpu::PipelineLayoutDescriptor layoutDesc{};
		layoutDesc.label = { "LightShaderPipelineLayout", WGPU_STRLEN };
		layoutDesc.bindGroupLayoutCount = std::size(bindGroupLayouts);
		layoutDesc.bindGroupLayouts = reinterpret_cast<WGPUBindGroupLayout*>(bindGroupLayouts);
		pipelineDesc.layout = GraphicsContext::GetDevice().createPipelineLayout(layoutDesc);

		m_LightPipeline = GraphicsContext::GetDevice().createRenderPipeline(pipelineDesc);
	}

	void LightPass::Execute(wgpu::CommandEncoder& encoder, const RenderContext& context, const DrawList& drawList)
	{
		if (!m_Enabled) return;

		wgpu::RenderPassColorAttachment color{};
		color.view = context.colorTarget;
		color.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		color.loadOp = wgpu::LoadOp::Load;
		color.storeOp = wgpu::StoreOp::Store;
		color.clearValue = wgpu::Color(0.0f, 0.0f, 0.0f, 0.0f);

		wgpu::RenderPassDepthStencilAttachment depth{};
		depth.view = context.depthTarget;
		depth.depthClearValue = 1.0f;
		depth.depthLoadOp = wgpu::LoadOp::Load;
		depth.depthStoreOp = wgpu::StoreOp::Store;
		depth.depthReadOnly = false;
		depth.stencilClearValue = 0;
		depth.stencilLoadOp = wgpu::LoadOp::Undefined;
		depth.stencilStoreOp = wgpu::StoreOp::Undefined;
		depth.stencilReadOnly = true;

		wgpu::RenderPassDescriptor renderPassDescriptor;
		renderPassDescriptor.label = { "LightRenderPass", WGPU_STRLEN };
		renderPassDescriptor.colorAttachmentCount = 1;
		renderPassDescriptor.colorAttachments = &color;
		renderPassDescriptor.depthStencilAttachment = &depth;

		wgpu::RenderPassEncoder pass = encoder.beginRenderPass(renderPassDescriptor);
		pass.setPipeline(m_LightPipeline);

		pass.setBindGroup(0, RenderGlobals::GetFrameBindGroup(), 0, nullptr);
		pass.setBindGroup(2, s_LightBindGroup, 0, nullptr);




		auto lightEntities = context.scene->GetEntities<LightComponent>();

		LightUniforms lightUniforms{};
		lightUniforms.count = static_cast<uint32_t>(lightEntities.size());

		int i = 0;
		for (auto entity : lightEntities)
		{
			auto& lightComponent = entity.GetComponent<LightComponent>();
			auto& transform = entity.GetComponent<TransformComponent>();

			Light light{
				.position = transform.position,
				.range = lightComponent.range,
				.color = lightComponent.color,
				.intensity = lightComponent.intensity
			};


			lightUniforms.lights[i++] = light;
		}

		GraphicsContext::GetDevice().getQueue().writeBuffer(s_LightUniformBuffer, 0, &lightUniforms, sizeof(LightUniforms));


		Mesh* mesh = nullptr;

		for (const DrawItem& draw : drawList.items)
		{
			if (!draw.mesh) continue;

			if (draw.mesh != mesh)
			{
				mesh = draw.mesh;
				pass.setVertexBuffer(0, mesh->GetVertexBuffer(), 0, mesh->GetVertexBuffer().getSize());
				pass.setIndexBuffer(mesh->GetIndexBuffer(), wgpu::IndexFormat::Uint32, 0, mesh->GetIndexBuffer().getSize());
			}

			uint32_t dynamicOffset = draw.modelIndex * RenderGlobals::GetModelUniformStride();
			pass.setBindGroup(1, RenderGlobals::GetModelBindGroup(), 1, &dynamicOffset);
			pass.drawIndexed(static_cast<uint32_t>(mesh->GetIndexBuffer().getSize() / sizeof(uint32_t)), 1, 0, 0, 0);
		}
		pass.end();
	}
}