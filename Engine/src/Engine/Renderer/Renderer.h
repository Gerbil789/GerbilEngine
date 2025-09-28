#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Renderer/Camera.h"
#include <webgpu/webgpu.hpp>

namespace Engine
{
	class Renderer //TODO: move static resources to RenderCore namespace or smth, probably create another file, this is getting bloated
	{
	public:
		static void Initialize(); //TODO: why is this static?
		Renderer(uint32_t width, uint32_t height);

		static Ref<Texture2D> GetDefaultWhiteTexture() { return s_DefaultWhite; }
		static Ref<Material> GetInvalidMaterial() { return s_InvalidMaterial; }

		void SetScene(Scene* scene) { m_Scene = scene; }
		void Resize(uint32_t width, uint32_t height);

		void SetCamera(Camera* camera) { m_Camera = camera; }

		void BeginScene();
		void RenderScene();
		void EndScene();

		wgpu::TextureView GetTextureView() const { return m_TextureView; }

		static wgpu::BindGroupLayout GetModelBindGroupLayout() { return s_ModelBindGroupLayout; }
		static wgpu::BindGroupLayout GetFrameBindGroupLayout() { return s_FrameBindGroupLayout; }

		static wgpu::BindGroup GetModelBindGroup() { return s_ModelBindGroup; }
		static wgpu::BindGroup GetFrameBindGroup() { return s_FrameBindGroup; }

		static wgpu::Buffer GetModelUniformBuffer() { return s_ModelUniformBuffer; }
		static wgpu::Buffer GetFrameUniformBuffer() { return s_FrameUniformBuffer; }

		static uint32_t GetModelUniformStride() { return s_ModelUniformStride; }

		// Global sampler (tmp)
		static wgpu::Sampler s_Sampler; //TODO: move into shader or material class

	private:
		Scene* m_Scene;
		Camera* m_Camera;


		static wgpu::Device s_Device;
		static wgpu::Queue s_Queue;
		wgpu::TextureView m_TextureView;
		wgpu::TextureView m_DepthView;
		wgpu::CommandEncoder m_CommandEncoder;
		wgpu::RenderPassEncoder m_RenderPass;

		// Model bind group
		static wgpu::BindGroupLayout s_ModelBindGroupLayout;
		static wgpu::BindGroup s_ModelBindGroup;
		static wgpu::Buffer s_ModelUniformBuffer;
		static uint32_t s_ModelUniformStride;

		// Frame bind group
		static wgpu::BindGroupLayout s_FrameBindGroupLayout;
		static wgpu::BindGroup s_FrameBindGroup;
		static wgpu::Buffer s_FrameUniformBuffer;

		// static resources
		inline static Ref<Texture2D> s_DefaultWhite = nullptr;
		inline static Ref<Material>  s_InvalidMaterial = nullptr;
		inline static wgpu::Sampler  s_DefaultSampler = nullptr;
	};
}