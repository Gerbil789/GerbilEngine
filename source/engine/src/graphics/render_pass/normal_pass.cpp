#include "enginepch.h"
//#include "engine/graphics/Renderpass/Normalpass.h"
//#include "engine/graphics/mesh.h"
//#include "engine/graphics/utility.h"
//#include "engine/graphics/graphics_context.h"
//#include "engine/graphics/renderer/render_pipeline_layouts.h"
//#include "engine/asset/asset_manager.h"
//#include "engine/utility/file.h"
//
//#include "engine/graphics/pipeline.h"
//#include "engine/asset/Resources.h"
//
//namespace engine
//{
//	wgpu::RenderPipeline m_pipeline;
//
//	Normalpass::Normalpass()
//	{
//		PipelineSpecification spec;
//		spec.shader = RESOURCES::SHADER::NORMAL;
//		spec.topology = wgpu::PrimitiveTopology::TriangleList;
//		spec.frontFace = wgpu::FrontFace::CCW;
//		spec.cullMode = wgpu::CullMode::None;
//		spec.depthWrite = true;
//		spec.depthCompare = wgpu::CompareFunction::LessEqual;
//		spec.depthFormat = wgpu::textureFormat::Depth24Plus;
//
//		spec.layoutOverrides = { RenderPipelineLayouts::GetViewLayout(), RenderPipelineLayouts::GetModelLayout() };
//
//		m_pipeline = PipelineCache::GetPipeline(spec);
//	}
//
//	void Normalpass::Execute(wgpu::commandEncoder& encoder, const FrameContext& context)
//	{
//		wgpu::RenderpassColorAttachment color;
//		color.view = context.colorTarget;
//		color.loadOp = wgpu::LoadOp::Load;
//		color.storeOp = wgpu::StoreOp::Store;
//		color.clearValue = wgpu::Color(0.0f, 0.0f, 0.0f, 0.0f);
//
//		wgpu::RenderpassDepthStencilAttachment depth;
//		depth.view = context.depthTarget;
//		depth.depthClearValue = 1.0f;
//		depth.depthLoadOp = wgpu::LoadOp::Load;
//		depth.depthStoreOp = wgpu::StoreOp::Store;
//		depth.depthReadOnly = false;
//		depth.stencilClearValue = 0;
//		depth.stencilLoadOp = wgpu::LoadOp::Undefined;
//		depth.stencilStoreOp = wgpu::StoreOp::Undefined;
//		depth.stencilReadOnly = true;
//
//		wgpu::RenderpassDescriptor renderpassDescriptor;
//		renderpassDescriptor.label = "NormalRenderpass";
//		renderpassDescriptor.colorAttachmentCount = 1;
//		renderpassDescriptor.colorAttachments = &color;
//		renderpassDescriptor.depthStencilAttachment = &depth;
//
//		wgpu::RenderpassEncoder pass = encoder.BeginRenderpass(&renderpassDescriptor);
//		pass.SetPipeline(m_pipeline);
//
//		pass.SetBindGroup(0, context.viewBindGroup, 0, nullptr);
//		pass.SetBindGroup(1, context.modelBindGroup, 0, nullptr);
//
//		Mesh lastMesh;
//
//		size_t i = 0;
//		for (const auto& item : context.drawList.GetItems())
//		{
//			if (item.mesh != lastMesh)
//			{
//				lastMesh = item.mesh;
//				const Meshasset& mesh = engine::assetManager::Getasset(lastMesh);
//				pass.SetVertexBuffer(0, mesh.GetVertexBuffer(), 0, mesh.GetVertexBuffer().GetSize());
//				pass.SetIndexBuffer(mesh.GetIndexBuffer(), wgpu::IndexFormat::Uint32, 0, mesh.GetIndexBuffer().GetSize());
//			}
//
//			pass.DrawIndexed(item.indexCount, 1, item.firstIndex, 0, static_cast<uint32_t>(i));
//			i++;
//		}
//
//		pass.End();
//	}
//}