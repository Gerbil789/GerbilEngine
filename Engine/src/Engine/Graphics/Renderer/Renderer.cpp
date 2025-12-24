#include "enginepch.h"
#include "Renderer.h"
#include "Engine/Graphics/GraphicsContext.h"
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

	Renderer::Renderer(uint32_t width, uint32_t height)
	{
		Resize(width, height);
	}

	Renderer::~Renderer()
	{

	}

	void Renderer::Resize(uint32_t width, uint32_t height)
	{
		ENGINE_PROFILE_FUNCTION();

		// Color texture
		wgpu::TextureDescriptor colorTextureDesc{};
		colorTextureDesc.label = { "RendererColorTexture" };
		colorTextureDesc.dimension = wgpu::TextureDimension::_2D;
		colorTextureDesc.format = wgpu::TextureFormat::RGBA8Unorm;
		colorTextureDesc.size = { width, height, 1 };
		colorTextureDesc.mipLevelCount = 1;
		colorTextureDesc.sampleCount = 1;
		colorTextureDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;
		wgpu::Texture colorTexture = GraphicsContext::GetDevice().createTexture(colorTextureDesc);

		wgpu::TextureViewDescriptor viewDesc{};
		viewDesc.label = { "RendererColorTextureView" };
		viewDesc.dimension = WGPUTextureViewDimension_2D;
		viewDesc.format = colorTextureDesc.format;
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = 1;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 1;
		m_RenderContext.colorTarget = colorTexture.createView(viewDesc);

		// Depth texture
		wgpu::TextureDescriptor depthTextureDesc;
		depthTextureDesc.label = { "RendererDepthTextureView" };
		depthTextureDesc.dimension = wgpu::TextureDimension::_2D;
		depthTextureDesc.format = wgpu::TextureFormat::Depth24Plus;
		depthTextureDesc.mipLevelCount = 1;
		depthTextureDesc.sampleCount = 1;
		depthTextureDesc.size = { width, height, 1 };
		depthTextureDesc.usage = wgpu::TextureUsage::RenderAttachment;
		depthTextureDesc.viewFormatCount = 1;
		depthTextureDesc.viewFormats = (WGPUTextureFormat*)&wgpu::TextureFormat::Depth24Plus;
		wgpu::Texture depthTexture = GraphicsContext::GetDevice().createTexture(depthTextureDesc);

		wgpu::TextureViewDescriptor depthTextureViewDesc;
		depthTextureViewDesc.aspect = wgpu::TextureAspect::DepthOnly;
		depthTextureViewDesc.baseArrayLayer = 0;
		depthTextureViewDesc.arrayLayerCount = 1;
		depthTextureViewDesc.baseMipLevel = 0;
		depthTextureViewDesc.mipLevelCount = 1;
		depthTextureViewDesc.dimension = wgpu::TextureViewDimension::_2D;
		depthTextureViewDesc.format = wgpu::TextureFormat::Depth24Plus;
		m_RenderContext.depthTarget = depthTexture.createView(depthTextureViewDesc);

		m_RenderContext.width = width;
		m_RenderContext.height = height;
	}

	void Renderer::RenderScene()
	{
		ENGINE_PROFILE_FUNCTION();
		wgpu::CommandEncoderDescriptor encoderDesc = {};
		encoderDesc.label = { "RendererCommandEncoder" };
		m_CommandEncoder = GraphicsContext::GetDevice().createCommandEncoder(encoderDesc);

		RenderGlobals::FrameUniforms frameUniforms;
		frameUniforms.view = m_Camera->GetViewMatrix();
		frameUniforms.projection = m_Camera->GetProjectionMatrix();
		frameUniforms.cameraPosition = m_Camera->GetPosition();
		GraphicsContext::GetQueue().writeBuffer(RenderGlobals::GetFrameUniformBuffer(), 0, &frameUniforms, sizeof(frameUniforms));

		for(auto pass : m_Passes)
		{
			pass->Execute(m_CommandEncoder, m_RenderContext, m_Scene, m_Camera);
		}

		wgpu::CommandBuffer commandBuffer = m_CommandEncoder.finish();
		m_CommandEncoder.release();
		GraphicsContext::GetQueue().submit(1, &commandBuffer);
	}
}