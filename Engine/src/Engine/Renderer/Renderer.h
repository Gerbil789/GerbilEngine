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

		inline static wgpu::Sampler s_Sampler = nullptr; //TODO: move into shader or material class

	private:
		Scene* m_Scene;
		Camera* m_Camera;

		wgpu::TextureView m_TextureView;
		wgpu::TextureView m_DepthView;
		wgpu::CommandEncoder m_CommandEncoder;
		wgpu::RenderPassEncoder m_RenderPass;

		inline static wgpu::Device s_Device = nullptr;
		inline static wgpu::Queue s_Queue = nullptr;

		// Model bind group
		inline static wgpu::BindGroupLayout s_ModelBindGroupLayout = nullptr;
		inline static wgpu::BindGroup s_ModelBindGroup = nullptr;
		inline static wgpu::Buffer s_ModelUniformBuffer = nullptr;
		inline static uint32_t s_ModelUniformStride = 0;

		// Frame bind group
		inline static wgpu::BindGroupLayout s_FrameBindGroupLayout = nullptr;
		inline static wgpu::BindGroup s_FrameBindGroup = nullptr;
		inline static wgpu::Buffer s_FrameUniformBuffer = nullptr;

		// Static resources
		inline static Ref<Texture2D> s_DefaultWhite = nullptr;
	};
}