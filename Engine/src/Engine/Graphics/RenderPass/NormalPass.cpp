#include "enginepch.h"
#include "NormalPass.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"
#include "Engine/Utils/File.h"

namespace Engine
{
	wgpu::RenderPipeline m_NormalPipeline;

	NormalPass::NormalPass()
	{
		std::string content;
		if (!Engine::ReadFile("Resources/Engine/shaders/normal.wgsl", content))
		{
			throw std::runtime_error("Failed to load normal shader");
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
		pipelineDesc.label = { "NormalShaderPipeline", WGPU_STRLEN };

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

		wgpu::ColorTargetState colorTarget;
		colorTarget.format = wgpu::TextureFormat::RGBA8Unorm;
		//colorTarget.blend = &blendState;
		colorTarget.writeMask = wgpu::ColorWriteMask::All;


		wgpu::DepthStencilState depthStencil{};
		depthStencil.format = wgpu::TextureFormat::Depth24Plus;
		depthStencil.depthWriteEnabled = wgpu::OptionalBool::False;
		depthStencil.depthCompare = wgpu::CompareFunction::LessEqual;
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
		pipelineDesc.multisample.mask = ~0u; // Default value for the mask, meaning "all bits on"
		pipelineDesc.multisample.alphaToCoverageEnabled = false;

		wgpu::BindGroupLayout bindGroupLayouts[] = {
			RenderGlobals::GetFrameLayout(),
			RenderGlobals::GetModelLayout()
		};

		wgpu::PipelineLayoutDescriptor layoutDesc{};
		layoutDesc.label = { "NormalShaderPipelineLayout", WGPU_STRLEN };
		layoutDesc.bindGroupLayoutCount = 2;
		layoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)&bindGroupLayouts;
		pipelineDesc.layout = GraphicsContext::GetDevice().createPipelineLayout(layoutDesc);

		m_NormalPipeline = GraphicsContext::GetDevice().createRenderPipeline(pipelineDesc);
	}

	void NormalPass::Execute(wgpu::CommandEncoder& encoder, const RenderContext& context, const DrawList& drawList)
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
		renderPassDescriptor.label = { "NormalRenderPass", WGPU_STRLEN };
		renderPassDescriptor.colorAttachmentCount = 1;
		renderPassDescriptor.colorAttachments = &color;
		renderPassDescriptor.depthStencilAttachment = &depth;

		wgpu::RenderPassEncoder pass = encoder.beginRenderPass(renderPassDescriptor);
		pass.setPipeline(m_NormalPipeline);

		pass.setBindGroup(GroupID::Frame, RenderGlobals::GetFrameBindGroup(), 0, nullptr);

		Mesh* currentMesh = nullptr;

		for (const DrawItem& draw : drawList.items)
		{
			if (!draw.mesh) continue;

			if (draw.mesh != currentMesh)
			{
				pass.setVertexBuffer(0, draw.mesh->GetVertexBuffer(), 0, draw.mesh->GetVertexBuffer().getSize());
				pass.setIndexBuffer(draw.mesh->GetIndexBuffer(), wgpu::IndexFormat::Uint16, 0, draw.mesh->GetIndexBuffer().getSize());
				currentMesh = draw.mesh;
			}

			uint32_t dynamicOffset = draw.modelIndex * RenderGlobals::GetModelUniformStride();
			pass.setBindGroup(GroupID::Model, RenderGlobals::GetModelBindGroup(), 1, &dynamicOffset);
			pass.drawIndexed(currentMesh->GetIndexCount(), 1, 0, 0, 0);
		}
		pass.end();
	}
}