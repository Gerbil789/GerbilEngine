#include "enginepch.h"
//#include "Engine/Graphics/RenderPass/NormalPass.h"
//#include "Engine/Graphics/Mesh.h"
//#include "Engine/Graphics/Utility.h"
//#include "Engine/Graphics/GraphicsContext.h"
//#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"
//#include "Engine/Asset/AssetManager.h"
//#include "Engine/Utility/File.h"
//
//#include "Engine/Graphics/Pipeline.h"
//#include "Engine/Asset/Resources.h"
//
//namespace engine
//{
//	wgpu::RenderPipeline m_Pipeline;
//
//	NormalPass::NormalPass()
//	{
//		PipelineSpecification spec;
//		spec.shader = RESOURCES::SHADER::NORMAL;
//		spec.topology = wgpu::PrimitiveTopology::TriangleList;
//		spec.frontFace = wgpu::FrontFace::CCW;
//		spec.cullMode = wgpu::CullMode::None;
//		spec.depthWrite = true;
//		spec.depthCompare = wgpu::CompareFunction::LessEqual;
//		spec.depthFormat = wgpu::TextureFormat::Depth24Plus;
//
//		spec.layoutOverrides = { RenderPipelineLayouts::GetViewLayout(), RenderPipelineLayouts::GetModelLayout() };
//
//		m_Pipeline = PipelineCache::GetPipeline(spec);
//	}
//
//	void NormalPass::Execute(wgpu::CommandEncoder& encoder, const FrameContext& context)
//	{
//		wgpu::RenderPassColorAttachment color;
//		color.view = context.colorTarget;
//		color.loadOp = wgpu::LoadOp::Load;
//		color.storeOp = wgpu::StoreOp::Store;
//		color.clearValue = wgpu::Color(0.0f, 0.0f, 0.0f, 0.0f);
//
//		wgpu::RenderPassDepthStencilAttachment depth;
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
//		wgpu::RenderPassDescriptor renderPassDescriptor;
//		renderPassDescriptor.label = "NormalRenderPass";
//		renderPassDescriptor.colorAttachmentCount = 1;
//		renderPassDescriptor.colorAttachments = &color;
//		renderPassDescriptor.depthStencilAttachment = &depth;
//
//		wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDescriptor);
//		pass.SetPipeline(m_Pipeline);
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
//				const MeshAsset& mesh = engine::AssetManager::GetAsset(lastMesh);
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