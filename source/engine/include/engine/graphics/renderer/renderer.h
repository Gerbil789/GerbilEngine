#pragma once

#include "engine/graphics/renderer/render_pass.h"
#include <vector>

namespace engine
{
	class Renderer
	{
	public:
		void Initialize(uint32_t width, uint32_t height);

		template<typename T, typename... Args>
    T* Addpass(Args&&... args)
    {
      auto pass = std::make_unique<T>(std::forward<Args>(args)...);
      T* passPtr = pass.get();
      passPtr->Initialize();
      m_passes.push_back(std::move(pass));
      return passPtr;
    }

		void SetSize(uint32_t width, uint32_t height);
		void SetEnvironment(TextureHandle texture); //TODO: move away from renderer?

		void Render(const DrawList& drawList, const component::Camera* camera, wgpu::TextureView targetView);

	private:
		void CreateEnvironmentUniformBuffer();
		void CreateEnvironmentBindGroup();
		void CreateShadowtexture(); //TODO: move to shadow pass?

	private:
	  wgpu::Texture m_depthtexture;
		wgpu::Texture m_colortexture;

		wgpu::Buffer m_viewUniformBuffer;
		wgpu::Buffer m_modelStorageBuffer;
		wgpu::Buffer m_environmentUniformBuffer;

		FrameContext m_frameContext;

		std::vector<std::unique_ptr<IRenderpass>> m_passes;
	};
}