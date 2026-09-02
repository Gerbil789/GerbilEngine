#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"

namespace engine
{
	class NormalPass : public RenderPass
	{
	public:
		NormalPass();
		virtual void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context) override;
	};
}