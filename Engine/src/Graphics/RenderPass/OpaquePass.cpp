#include "enginepch.h"
#include "Engine/Graphics/RenderPass/OpaquePass.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"

namespace Engine
{
	//static AABB TransformAABB(const AABB& local, const glm::mat4& m)
//{
//	glm::vec3 center = (local.min + local.max) * 0.5f;
//	glm::vec3 extent = (local.max - local.min) * 0.5f;

//	glm::vec3 worldCenter = glm::vec3(m * glm::vec4(center, 1.0f));

//	glm::mat3 absMat = glm::mat3(
//		glm::abs(m[0]),
//		glm::abs(m[1]),
//		glm::abs(m[2])
//	);

//	glm::vec3 worldExtent = absMat * extent;

//	return {
//			worldCenter - worldExtent,
//			worldCenter + worldExtent
//	};
//}

//struct Frustum
//{
//	glm::vec4 planes[6]; // (normal.xyz, distance)
//};

//static Frustum ExtractFrustum(const glm::mat4& vp)
//{
//	Frustum f;

//	// vp = projection * view
//	f.planes[0] = vp[3] + vp[0]; // Left
//	f.planes[1] = vp[3] - vp[0]; // Right
//	f.planes[2] = vp[3] + vp[1]; // Bottom
//	f.planes[3] = vp[3] - vp[1]; // Top
//	f.planes[4] = vp[3] + vp[2]; // Near
//	f.planes[5] = vp[3] - vp[2]; // Far

//	// Normalize planes
//	for (auto& p : f.planes)
//	{
//		float len = glm::length(glm::vec3(p));
//		p = -p / len; // flip so normal points into frustum
//	}

//	return f;
//}

//static bool IsVisible(const Frustum& f, const AABB& aabb)
//{
//	for (const auto& plane : f.planes)
//	{
//		glm::vec3 normal = glm::vec3(plane);

//		glm::vec3 p = aabb.min;
//		if (plane.x >= 0) p.x = aabb.max.x;
//		if (plane.y >= 0) p.y = aabb.max.y;
//		if (plane.z >= 0) p.z = aabb.max.z;

//		if (glm::dot(normal, p) + plane.w < 0.001f)
//		{
//			return false;
//		}
//	}

//	return true;
//}

	void OpaquePass::Execute(wgpu::CommandEncoder& encoder, const RenderContext& context, const DrawList& drawList)
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
		depth.depthLoadOp = wgpu::LoadOp::Clear;
		depth.depthStoreOp = wgpu::StoreOp::Store;
		depth.depthReadOnly = false;
		depth.stencilClearValue = 0;
		depth.stencilLoadOp = wgpu::LoadOp::Undefined;
		depth.stencilStoreOp = wgpu::StoreOp::Undefined;
		depth.stencilReadOnly = true;

		wgpu::RenderPassDescriptor passDescriptor{};
		passDescriptor.label = { "OpaqueRenderPass", WGPU_STRLEN };
		passDescriptor.colorAttachmentCount = 1;
		passDescriptor.colorAttachments = &color;
		passDescriptor.depthStencilAttachment = &depth;

		wgpu::RenderPassEncoder pass = encoder.beginRenderPass(passDescriptor);

		pass.setBindGroup(GroupID::Frame, RenderGlobals::GetFrameBindGroup(), 0, nullptr);

		Material* currentMaterial = nullptr;
		Mesh* currentMesh = nullptr;

		for (const DrawItem& draw : drawList.items)
		{
			if (!draw.mesh) continue;

			if (draw.material != currentMaterial)
			{
				draw.material->Bind(pass);
				pass.setPipeline(draw.material->GetShader()->GetRenderPipeline());
				currentMaterial = draw.material;
				currentMesh = nullptr;
			}

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