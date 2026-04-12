#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"
#include <glm/glm.hpp>

namespace Engine
{
	struct alignas(16) EnvironmentUniforms //TODO: i dont like having this here...
	{
		std::array<glm::mat4, s_ShadowCascadeCount> lightViewProj;
		std::array<float, s_ShadowCascadeCount> cascadeSplits;
	};
	static_assert(sizeof(EnvironmentUniforms) % 16 == 0);

	class Camera;
	class Scene;
	class CubeMapTexture;

	class ENGINE_API Renderer
	{
	public:
		static void InitializeSharedResources();
		void Initialize();
		void AddPass(RenderPass* pass);
		void RemovePass(RenderPass* pass);

		void SetCamera(Camera* camera);
		void Resize(uint32_t width, uint32_t height); //TODO: resize should be handled outside of the renderer, by setting new targets
		void SetColorTarget(wgpu::TextureView colorView);
		//void SetDepthTarget(wgpu::TextureView depthView);
		CubeMapTexture* GetSkyboxCubemap() const { return m_RenderContext.environmentCubemap; }
		void SetSkyboxCubemap(CubeMapTexture* cubemap) { m_RenderContext.environmentCubemap = cubemap; CreateEnvironmentBindGroup();}

		void RenderScene(Scene* scene);
		wgpu::TextureView GetTextureView() const;

		static wgpu::BindGroupLayout GetViewLayout();
		static wgpu::BindGroupLayout GetModelLayout();
		static wgpu::BindGroupLayout GetEnvironmentLayout();

	private:
		static void CreateViewBindGroupLayout();
		void CreateViewUniformBuffer();
		void CreateViewBindGroup();

		static void CreateModelBindGroupLayout();
		void CreateModelUniformBuffer();
		void CreateModelBindGroup();

		static void CreateEnvironmentBindGroupLayout();
		void CreateEnvironmentUniformBuffer();
		void CreateEnvironmentBindGroup();

		void CreateShadowTexture();

	private:
		RenderContext m_RenderContext;
		std::vector<RenderPass*> m_Passes;
	};
}