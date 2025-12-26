#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"

namespace Engine
{
	class BackgroundPass : public RenderPass
	{
	public:
		virtual void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context) override;
	};
}