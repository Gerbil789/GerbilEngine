#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"

namespace engine
{
	class ShadowPass : public RenderPass
	{
	public:
		ShadowPass();
		virtual void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context) override;

		static float s_Lambda;
	};
}