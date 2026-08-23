#include "enginepch.h"
#include "Engine/Graphics/RenderPass/NormalPass.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Utility.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Utility/File.h"

namespace Engine
{
	wgpu::RenderPipeline m_NormalPipeline;

	NormalPass::NormalPass()
	{
		wgpu::ShaderModule shaderModule = LoadWGSLShader("resources/shaders/normal.wgsl");
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

		vertexBufferLayout.attributeCount = vertexAttribs.size();
		vertexBufferLayout.attributes = vertexAttribs.data();
		vertexBufferLayout.arrayStride = 8 * sizeof(float);
		vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;

		wgpu::RenderPipelineDescriptor pipelineDesc;
		pipelineDesc.label = "NormalShaderPipeline";

		pipelineDesc.vertex.bufferCount = 1;
		pipelineDesc.vertex.buffers = &vertexBufferLayout;
		pipelineDesc.vertex.module = shaderModule;
		pipelineDesc.vertex.entryPoint = "vs_main";
		pipelineDesc.vertex.constantCount = 0;
		pipelineDesc.vertex.constants = nullptr;

		pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
		pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
		pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
		pipelineDesc.primitive.cullMode = wgpu::CullMode::None;

		wgpu::ColorTargetState colorTarget;
		colorTarget.format = GraphicsContext::GetSurfaceFormat();
		//colorTarget.blend = &blendState;
		colorTarget.writeMask = wgpu::ColorWriteMask::All;

		wgpu::DepthStencilState depthStencil;
		depthStencil.format = wgpu::TextureFormat::Depth24Plus;
		depthStencil.depthWriteEnabled = wgpu::OptionalBool::False;
		depthStencil.depthCompare = wgpu::CompareFunction::LessEqual;
		depthStencil.stencilFront = {};
		depthStencil.stencilBack = {};
		depthStencil.stencilReadMask = 0;
		depthStencil.stencilWriteMask = 0;

		wgpu::FragmentState fragmentState;
		fragmentState.module = shaderModule;
		fragmentState.entryPoint = "fs_main";
		fragmentState.constantCount = 0;
		fragmentState.constants = nullptr;
		fragmentState.targetCount = 1;
		fragmentState.targets = &colorTarget;
		pipelineDesc.depthStencil = &depthStencil;
		pipelineDesc.fragment = &fragmentState;

		pipelineDesc.multisample.count = 1;
		pipelineDesc.multisample.mask = ~0u;
		pipelineDesc.multisample.alphaToCoverageEnabled = false;

		std::array<wgpu::BindGroupLayout, 2> bindGroupLayouts 
		{
			RenderPipelineLayouts::GetViewLayout(),
			RenderPipelineLayouts::GetModelLayout()
		};

		wgpu::PipelineLayoutDescriptor layoutDesc;
		layoutDesc.label = "NormalShaderPipelineLayout";
		layoutDesc.bindGroupLayoutCount = bindGroupLayouts.size();
		layoutDesc.bindGroupLayouts = bindGroupLayouts.data();
		pipelineDesc.layout = GraphicsContext::GetDevice().CreatePipelineLayout(&layoutDesc);

		m_NormalPipeline = GraphicsContext::GetDevice().CreateRenderPipeline(&pipelineDesc);
	}

	void NormalPass::Execute(wgpu::CommandEncoder& encoder, const RenderContext& context)
	{
		wgpu::RenderPassColorAttachment color;
		color.view = context.colorTarget;
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
		renderPassDescriptor.label = "NormalRenderPass";
		renderPassDescriptor.colorAttachmentCount = 1;
		renderPassDescriptor.colorAttachments = &color;
		renderPassDescriptor.depthStencilAttachment = &depth;

		wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDescriptor);
		pass.SetPipeline(m_NormalPipeline);

		pass.SetBindGroup(0, context.viewBindGroup, 0, nullptr);
		pass.SetBindGroup(1, context.modelBindGroup, 0, nullptr);

		Mesh lastMesh;

		size_t i = 0;
		for (const auto& item : context.drawList.GetItems())
		{
			if (item.mesh != lastMesh)
			{
				lastMesh = item.mesh;
				const MeshAsset& mesh = Engine::AssetManager::GetAsset(lastMesh);
				pass.SetVertexBuffer(0, mesh.GetVertexBuffer(), 0, mesh.GetVertexBuffer().GetSize());
				pass.SetIndexBuffer(mesh.GetIndexBuffer(), wgpu::IndexFormat::Uint32, 0, mesh.GetIndexBuffer().GetSize());
			}

			pass.DrawIndexed(item.indexCount, 1, item.firstIndex, 0, static_cast<uint32_t>(i));
			i++;
		}

		pass.End();
	}
}