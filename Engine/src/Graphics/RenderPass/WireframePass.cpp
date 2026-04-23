#include "enginepch.h"
#include "Engine/Graphics/RenderPass/WireframePass.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Utility/File.h"
#include "Engine/Graphics/WebGPUUtils.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"

namespace Engine
{
	namespace
	{
		struct WireframeUniform
		{
			glm::vec4 color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
		};
		static_assert(sizeof(WireframeUniform) % 16 == 0);

		wgpu::RenderPipeline m_WireframePipeline;
		wgpu::BindGroup m_ShadowBindGroup;
		wgpu::Buffer m_UniformBuffer;

		WireframeUniform m_UniformData;
	}

	WireframePass::WireframePass()
	{
		wgpu::BindGroupLayoutEntry entry;
		entry.binding = 0;
		entry.visibility = wgpu::ShaderStage::Fragment;
		entry.buffer.type = wgpu::BufferBindingType::Uniform;
		entry.buffer.minBindingSize = sizeof(WireframeUniform);
		entry.buffer.hasDynamicOffset = false;

		wgpu::BindGroupLayoutDescriptor desc;
		desc.label = { "WireframeBindGroupLayout", WGPU_STRLEN };
		desc.entryCount = 1;
		desc.entries = &entry;

		wgpu::BindGroupLayout bindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(desc);

		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.label = { "WireframeUniformBuffer", WGPU_STRLEN };
		bufferDesc.size = 1024 * 256 * sizeof(WireframeUniform); //1024 max entities, 256 bytes alignment
		bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;

		m_UniformBuffer = GraphicsContext::GetDevice().createBuffer(bufferDesc);

		wgpu::BindGroupEntry bindGroupEntry;
		bindGroupEntry.binding = 0;
		bindGroupEntry.buffer = m_UniformBuffer;
		bindGroupEntry.offset = 0;
		bindGroupEntry.size = sizeof(WireframeUniform);

		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.label = { "WireframeBindGroup", WGPU_STRLEN };
		bindGroupDesc.layout = bindGroupLayout;
		bindGroupDesc.entryCount = 1;
		bindGroupDesc.entries = &bindGroupEntry;
		m_ShadowBindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);

		wgpu::ShaderModule shaderModule = LoadWGSLShader("Resources/Engine/shaders/wireframe.wgsl");
		std::array<wgpu::VertexAttribute, 3> vertexAttribs;

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
		pipelineDesc.label = { "WireframeShaderPipeline", WGPU_STRLEN };

		pipelineDesc.vertex.bufferCount = 1;
		pipelineDesc.vertex.buffers = &vertexBufferLayout;
		pipelineDesc.vertex.module = shaderModule;
		pipelineDesc.vertex.entryPoint = { "vs_main", WGPU_STRLEN };
		pipelineDesc.vertex.constantCount = 0;
		pipelineDesc.vertex.constants = nullptr;

		pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::LineList;
		pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
		pipelineDesc.primitive.frontFace = wgpu::FrontFace::CW;
		pipelineDesc.primitive.cullMode = wgpu::CullMode::None;

		wgpu::BlendState blendState;
		blendState.color.srcFactor = wgpu::BlendFactor::One;
		blendState.color.dstFactor = wgpu::BlendFactor::Zero;
		blendState.color.operation = wgpu::BlendOperation::Add;
		blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
		blendState.alpha.dstFactor = wgpu::BlendFactor::One;
		blendState.alpha.operation = wgpu::BlendOperation::Add;

		wgpu::ColorTargetState colorTarget;
		colorTarget.format = wgpu::TextureFormat::RGBA8Unorm;
		colorTarget.blend = &blendState;
		colorTarget.writeMask = wgpu::ColorWriteMask::All;

		wgpu::DepthStencilState depthStencil;
		depthStencil.format = wgpu::TextureFormat::Depth24Plus;
		depthStencil.depthWriteEnabled = wgpu::OptionalBool::False;
		depthStencil.depthCompare = wgpu::CompareFunction::LessEqual;
		depthStencil.depthBias = 0;
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

		std::array<wgpu::BindGroupLayout, 3> bindGroupLayouts
		{
			RenderPipelineLayouts::GetViewLayout(),
			RenderPipelineLayouts::GetModelLayout(),
			bindGroupLayout
		};

		wgpu::PipelineLayoutDescriptor layoutDesc{};
		layoutDesc.label = { "WireframeShaderPipelineLayout", WGPU_STRLEN };
		layoutDesc.bindGroupLayoutCount = bindGroupLayouts.size();
		layoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)bindGroupLayouts.data();
		pipelineDesc.layout = GraphicsContext::GetDevice().createPipelineLayout(layoutDesc);

		m_WireframePipeline = GraphicsContext::GetDevice().createRenderPipeline(pipelineDesc);
	}

	void WireframePass::SetColor(const glm::vec4& color)
	{
		m_UniformData.color = color;
	}

	void WireframePass::Execute(wgpu::CommandEncoder& encoder, const RenderContext& context)
	{
		wgpu::RenderPassColorAttachment color;
		color.view = context.colorTarget;
		color.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		color.loadOp = wgpu::LoadOp::Load;
		color.storeOp = wgpu::StoreOp::Store;

		wgpu::RenderPassDepthStencilAttachment depth;
		depth.view = context.depthTarget;
		depth.depthClearValue = 1.0f;
		depth.depthLoadOp = wgpu::LoadOp::Clear;
		depth.depthStoreOp = wgpu::StoreOp::Discard;
		depth.depthReadOnly = false;
		depth.stencilClearValue = 0;
		depth.stencilLoadOp = wgpu::LoadOp::Undefined;
		depth.stencilStoreOp = wgpu::StoreOp::Undefined;
		depth.stencilReadOnly = true;

		wgpu::RenderPassDescriptor passDescriptor;
		passDescriptor.label = { "WireframeRenderPass", WGPU_STRLEN };
		passDescriptor.colorAttachmentCount = 1;
		passDescriptor.colorAttachments = &color;
		passDescriptor.depthStencilAttachment = &depth;

		wgpu::RenderPassEncoder pass = encoder.beginRenderPass(passDescriptor);

		pass.setPipeline(m_WireframePipeline);
		pass.setBindGroup(0, context.viewBindGroup, 0, nullptr);

		GraphicsContext::GetQueue().writeBuffer(m_UniformBuffer, 0, &m_UniformData, sizeof(WireframeUniform));
		pass.setBindGroup(2, m_ShadowBindGroup, 0, nullptr);

		Mesh* mesh = nullptr;

		for (const DrawItem& item : context.drawList)
		{
			if (!item.mesh) continue;

			if (item.mesh != mesh)
			{
				mesh = item.mesh;
				pass.setVertexBuffer(0, mesh->GetVertexBuffer(), 0, mesh->GetVertexBuffer().getSize());
				pass.setIndexBuffer(mesh->GetWireIndexBuffer(), wgpu::IndexFormat::Uint32, 0, mesh->GetWireIndexBuffer().getSize());
			}

			const SubMesh* sub = item.subMesh;

			uint32_t dynamicOffset = item.modelIndex * GraphicsContext::GetUniformBufferOffsetAlignment();
			pass.setBindGroup(1, context.modelBindGroup, 1, &dynamicOffset);

			pass.drawIndexed(sub->indexCount * 2, 1, sub->firstIndex * 2, 0, 0);
		}
		pass.end();
	}
}