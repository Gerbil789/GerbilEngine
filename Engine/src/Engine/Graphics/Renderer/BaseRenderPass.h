#pragma once

#include "Engine/Graphics/Renderer/RenderPass.h"

namespace Engine
{
	class BaseRenderPass : public RenderPass
	{
	public:
		BaseRenderPass();
		virtual ~BaseRenderPass();

		virtual void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context, Scene* scene, Camera* camera) override;
	};
}