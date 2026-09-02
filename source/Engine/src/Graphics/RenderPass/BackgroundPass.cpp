#include "enginepch.h"
#include "Engine/Graphics/RenderPass/BackgroundPass.h"
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"
#include "Engine/Graphics/Utility.h"

namespace engine
{
	namespace
	{
		wgpu::RenderPipeline m_Pipeline;
	}

	void CreatePipeline()
	{
		wgpu::ShaderModule shaderModule = LoadWGSLShader("resources/shaders/skybox.wgsl");

		wgpu::RenderPipelineDescriptor pipelineDesc;
		pipelineDesc.label = "SkyboxShaderPipeline";

		pipelineDesc.vertex.bufferCount = 0;
		//pipelineDesc.vertex.buffers = &vertexBufferLayout;
		pipelineDesc.vertex.module = shaderModule;
		pipelineDesc.vertex.entryPoint = "vs_main";
		pipelineDesc.vertex.constantCount = 0;
		pipelineDesc.vertex.constants = nullptr;

		pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
		pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
		pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
		pipelineDesc.primitive.cullMode = wgpu::CullMode::None;

		wgpu::BlendState blendState;
		blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
		blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
		blendState.color.operation = wgpu::BlendOperation::Add;
		blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
		blendState.alpha.dstFactor = wgpu::BlendFactor::One;
		blendState.alpha.operation = wgpu::BlendOperation::Add;

		wgpu::ColorTargetState colorTarget;
		colorTarget.format = GraphicsContext::GetSurfaceFormat();
		colorTarget.blend = &blendState;
		colorTarget.writeMask = wgpu::ColorWriteMask::All;

		wgpu::FragmentState fragmentState;
		fragmentState.module = shaderModule;
		fragmentState.entryPoint = "fs_main";
		fragmentState.constantCount = 0;
		fragmentState.constants = nullptr;
		fragmentState.targetCount = 1;
		fragmentState.targets = &colorTarget;
		pipelineDesc.fragment = &fragmentState;

		//wgpu::DepthStencilState depthStencilState{};
		//depthStencilState.depthCompare = wgpu::CompareFunction::LessEqual;
		//depthStencilState.depthWriteEnabled = wgpu::OptionalBool::False;
		//depthStencilState.format = wgpu::TextureFormat::Depth24Plus;
		//depthStencilState.stencilReadMask = 0xFFFFFFFF;
		//depthStencilState.stencilWriteMask = 0xFFFFFFFF;
		//pipelineDesc.depthStencil = &depthStencilState;

		pipelineDesc.multisample.count = 1;		// no multisampling
		pipelineDesc.multisample.mask = ~0u;	// all samples enabled
		pipelineDesc.multisample.alphaToCoverageEnabled = false;

		std::array<wgpu::BindGroupLayout, 2> bindGroupLayouts
		{
			RenderPipelineLayouts::GetViewLayout(),
			RenderPipelineLayouts::GetEnvironmentLayout()
		};

		wgpu::PipelineLayoutDescriptor layoutDesc;
		layoutDesc.label = "SkyboxPipelineLayout";
		layoutDesc.bindGroupLayoutCount = bindGroupLayouts.size();
		layoutDesc.bindGroupLayouts = bindGroupLayouts.data();
		pipelineDesc.layout = GraphicsContext::GetDevice().CreatePipelineLayout(&layoutDesc);

		m_Pipeline = GraphicsContext::GetDevice().CreateRenderPipeline(&pipelineDesc);
	}


	BackgroundPass::BackgroundPass()
	{
		CreatePipeline();
	}

	void BackgroundPass::Execute(wgpu::CommandEncoder& encoder, const RenderContext& context)
	{
		const glm::vec4& col = context.camera->clearColor;

		wgpu::RenderPassColorAttachment color;
		color.view = context.colorTarget;
		color.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		color.loadOp = wgpu::LoadOp::Clear;
		color.storeOp = wgpu::StoreOp::Store;
		color.clearValue = wgpu::Color{ col.r, col.g, col.b, col.a };

		wgpu::RenderPassDescriptor passDescriptor;
		passDescriptor.label = "BackgroundRenderPass";
		passDescriptor.colorAttachmentCount = 1;
		passDescriptor.colorAttachments = &color;

		wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&passDescriptor);

		pass.SetBindGroup(0, context.viewBindGroup, 0, nullptr);

		if (context.camera->projectionType == CameraComponent::Projection::Perspective)
		{
			if (context.camera->background == CameraComponent::Background::Skybox)
			{
				pass.SetPipeline(m_Pipeline);
				pass.SetBindGroup(1, context.environmentBindGroup, 0, nullptr);
				pass.Draw(36, 1, 0, 0);
			}
		}

		pass.End();
	}
}