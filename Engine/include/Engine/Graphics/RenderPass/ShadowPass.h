#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"

namespace Engine
{
	class ENGINE_API ShadowPass : public RenderPass
	{
	public:
		ShadowPass();
		virtual void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context, const DrawList& drawList) override;

		static void Initialize();

		static wgpu::TextureView GetShadowMap();

		static RenderGlobals::ShadowUniforms GetShadowUniforms();
	};
}