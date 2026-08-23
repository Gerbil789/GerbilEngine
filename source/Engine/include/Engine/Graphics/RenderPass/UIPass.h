#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"

namespace Engine
{
	class UIPass : public RenderPass
	{
	public:
		UIPass();
		virtual void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context) override;
	};
}