#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"

namespace engine
{
	class BackgroundPass : public RenderPass
	{
	public:
		BackgroundPass();
		void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context) override;
	};
}