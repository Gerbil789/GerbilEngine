#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"

namespace Engine
{
	class NormalPass : public RenderPass
	{
	public:
		NormalPass();
		virtual void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context, const DrawList& drawList) override;
	};
}