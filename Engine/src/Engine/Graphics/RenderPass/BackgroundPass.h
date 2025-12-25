#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"

namespace Engine
{
	class BackgroundPass : public RenderPass
	{
	public:
		BackgroundPass();
		virtual ~BackgroundPass();

		virtual void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context) override;
	};
}