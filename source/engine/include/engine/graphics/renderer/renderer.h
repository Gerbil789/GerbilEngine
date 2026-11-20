#pragma once

#include "engine/graphics/renderer/renderer_core.h"
#include "engine/graphics/renderer/render_pass.h"
#include <tuple>

namespace engine
{
	template<typename... Passes>
	struct Renderer : public RendererCore
	{
		void Initialize(uint32_t width, uint32_t height)
    {
			RendererCore::Initialize(width, height);

      std::apply([](auto&... pass) {
        (..., [](auto& p) {
          if constexpr (requires { p.Initialize(); }) p.Initialize();
        }(pass));
      }, passes);
    }

		template<typename T>
    T& GetPass()
    {
      return std::get<T>(passes);
    }

		void Render(const DrawList& drawList, const component::Camera* camera, wgpu::TextureView targetView)
		{
      m_frameContext.colorTarget = targetView;

      wgpu::CommandEncoder encoder = RendererCore::BeginFrame(camera, drawList);

      std::apply([&](auto&... pass) {
        (..., [&]() {
          if (pass.enabled) pass.Execute(encoder, m_frameContext);
        }());
      }, passes);

      RendererCore::EndFrame(encoder);
    }

	private:
		std::tuple<Passes...> passes;
	};
}