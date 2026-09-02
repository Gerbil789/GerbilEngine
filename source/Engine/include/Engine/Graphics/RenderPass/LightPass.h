#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"

namespace engine
{
	class LightPass : public RenderPass
	{
	public:
		LightPass();
		virtual void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context) override;
	};
}