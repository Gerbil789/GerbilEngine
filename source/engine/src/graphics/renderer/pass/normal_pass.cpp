#include "enginepch.h"
#include "engine/graphics/renderer/render_pass.h"
#include "engine/graphics/graphics_context.h"
#include "engine/graphics/renderer/layouts.h"
#include "engine/graphics/pipeline.h"

namespace engine
{
	wgpu::RenderPipeline m_pipeline;

	void NormalPass::Initialize()
	{
		PipelineSpecification spec;
		spec.shaderHandle = engine::shader::Normal;
		spec.topology = wgpu::PrimitiveTopology::TriangleList;
		spec.frontFace = wgpu::FrontFace::CCW;
		spec.cullMode = wgpu::CullMode::None;
		spec.depthWrite = true;
		spec.depthCompare = wgpu::CompareFunction::Always;
		spec.depthFormat = wgpu::TextureFormat::Depth24Plus;
		spec.layouts = { Layouts::viewBindGroupLayout, Layouts::modelBindGroupLayout };
		m_pipeline = PipelineCache::GetPipeline(spec);
	}

	void NormalPass::Execute(wgpu::CommandEncoder& encoder, const FrameContext& context)
	{
		wgpu::RenderPassColorAttachment color;
		color.view = context.colorTarget;
		color.loadOp = wgpu::LoadOp::Load;
		color.storeOp = wgpu::StoreOp::Store;
		color.clearValue = wgpu::Color(0.0f, 0.0f, 0.0f, 0.0f);

		wgpu::RenderPassDepthStencilAttachment depth;
		depth.view = context.depthTarget;
		depth.depthClearValue = 1.0f;
		depth.depthLoadOp = wgpu::LoadOp::Load;
		depth.depthStoreOp = wgpu::StoreOp::Store;
		depth.depthReadOnly = false;
		depth.stencilClearValue = 0;
		depth.stencilLoadOp = wgpu::LoadOp::Undefined;
		depth.stencilStoreOp = wgpu::StoreOp::Undefined;
		depth.stencilReadOnly = true;

		wgpu::RenderPassDescriptor renderpassDescriptor;
		renderpassDescriptor.label = "NormalRenderpass";
		renderpassDescriptor.colorAttachmentCount = 1;
		renderpassDescriptor.colorAttachments = &color;
		renderpassDescriptor.depthStencilAttachment = &depth;

		wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderpassDescriptor);
		pass.SetPipeline(m_pipeline);

		pass.SetBindGroup(0, context.viewBindGroup, 0, nullptr);
		pass.SetBindGroup(1, context.modelBindGroup, 0, nullptr);

		MeshHandle currentMesh;

		size_t i = 0;
		for (const auto& item : context.drawList.items)
		{
			if (item.mesh != currentMesh)
			{
				currentMesh = item.mesh;
				const Mesh& mesh = currentMesh.Get();
				pass.SetVertexBuffer(0, mesh.vertexBuffer, 0, mesh.vertexBuffer.GetSize());
				pass.SetIndexBuffer(mesh.indexBuffer, wgpu::IndexFormat::Uint32, 0, mesh.indexBuffer.GetSize());
			}

			pass.DrawIndexed(item.indexCount, 1, item.firstIndex, 0, static_cast<uint32_t>(i++));
		}

		pass.End();
	}
}