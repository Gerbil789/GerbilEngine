#include "enginepch.h"
#include "Engine/Graphics/RenderPass/ShadowPass.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Utility/File.h"
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
	namespace 
	{
		wgpu::RenderPipeline m_ShadowPipeline;
		wgpu::BindGroup m_BindGroup;
		static wgpu::TextureView m_DepthTextureView;
		static RenderGlobals::ShadowUniforms s_ShadowUniforms;
	}

	void ShadowPass::Initialize()
	{
		wgpu::TextureFormat format = wgpu::TextureFormat::Depth24Plus;

		wgpu::TextureDescriptor textureDesc;
		textureDesc.dimension = wgpu::TextureDimension::_2D;
		textureDesc.format = format;
		textureDesc.mipLevelCount = 1;
		textureDesc.sampleCount = 1;
		textureDesc.size = { 1024, 1024, 1 };
		textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc;
		textureDesc.viewFormatCount = 0;
		textureDesc.viewFormats = nullptr;
		wgpu::Texture texture = GraphicsContext::GetDevice().createTexture(textureDesc);

		wgpu::TextureViewDescriptor viewDesc;
		viewDesc.format = format;
		viewDesc.aspect = wgpu::TextureAspect::DepthOnly;
		viewDesc.dimension = wgpu::TextureViewDimension::_2D;
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = 1;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 1;

		m_DepthTextureView = texture.createView(viewDesc);
	}


	wgpu::ShaderModule CreateShaderModule()
	{
		std::filesystem::path shaderPath = "Resources/Engine/shaders/shadow.wgsl";
		std::string source;
		if (!Engine::ReadFile(shaderPath, source))
		{
			throw std::runtime_error("Failed to load shadow shader: " + shaderPath.string());
		}

		wgpu::ShaderSourceWGSL desc;
		desc.chain.next = nullptr;
		desc.chain.sType = wgpu::SType::ShaderSourceWGSL;
		desc.code = { source.c_str(), WGPU_STRLEN };

		wgpu::ShaderModuleDescriptor shaderDesc{};
		shaderDesc.nextInChain = &desc.chain;
		return GraphicsContext::GetDevice().createShaderModule(shaderDesc);
	}


	Engine::ShadowPass::ShadowPass()
	{
		wgpu::ShaderModule shaderModule = CreateShaderModule();

		std::array<wgpu::VertexAttribute, 3> vertexAttribs;

		// Position
		vertexAttribs[0].shaderLocation = 0; // @location(0)
		vertexAttribs[0].format = wgpu::VertexFormat::Float32x3;
		vertexAttribs[0].offset = 0;

		// Normal
		vertexAttribs[1].shaderLocation = 1; // @location(1)
		vertexAttribs[1].format = wgpu::VertexFormat::Float32x3;
		vertexAttribs[1].offset = 3 * sizeof(float);

		// UV
		vertexAttribs[2].shaderLocation = 2; // @location(2)
		vertexAttribs[2].format = wgpu::VertexFormat::Float32x2;
		vertexAttribs[2].offset = 6 * sizeof(float);

		wgpu::VertexBufferLayout vertexBufferLayout;

		vertexBufferLayout.attributeCount = static_cast<uint32_t>(vertexAttribs.size());
		vertexBufferLayout.attributes = vertexAttribs.data();
		vertexBufferLayout.arrayStride = 8 * sizeof(float);
		vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;


		wgpu::RenderPipelineDescriptor pipelineDesc;
		pipelineDesc.label = { "ShadowShaderPipeline", WGPU_STRLEN };

		pipelineDesc.vertex.bufferCount = 1;
		pipelineDesc.vertex.buffers = &vertexBufferLayout;
		pipelineDesc.vertex.module = shaderModule;
		pipelineDesc.vertex.entryPoint = { "vs_main", WGPU_STRLEN};
		pipelineDesc.vertex.constantCount = 0;
		pipelineDesc.vertex.constants = nullptr;

		pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
		pipelineDesc.primitive.frontFace = wgpu::FrontFace::CW;
		pipelineDesc.primitive.cullMode = wgpu::CullMode::None;

		wgpu::BlendState blendState;
		blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
		blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
		blendState.color.operation = wgpu::BlendOperation::Add;
		blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
		blendState.alpha.dstFactor = wgpu::BlendFactor::One;
		blendState.alpha.operation = wgpu::BlendOperation::Add;

		pipelineDesc.fragment = nullptr; // no fragment shader

		wgpu::DepthStencilState depthStencilState;
		depthStencilState.depthCompare = wgpu::CompareFunction::Less;
		depthStencilState.depthWriteEnabled = wgpu::OptionalBool::True;
		depthStencilState.format = wgpu::TextureFormat::Depth24Plus;
		depthStencilState.stencilReadMask = 0xFFFFFFFF;
		depthStencilState.stencilWriteMask = 0xFFFFFFFF;
		depthStencilState.depthBias = 2;
		depthStencilState.depthBiasSlopeScale = 2.0f;
		depthStencilState.depthBiasClamp = 0.0f;

		pipelineDesc.depthStencil = &depthStencilState;

		pipelineDesc.multisample.count = 1;		// no multisampling
		pipelineDesc.multisample.mask = ~0u;	// all samples enabled
		pipelineDesc.multisample.alphaToCoverageEnabled = false;

		wgpu::BindGroupLayoutEntry bindGroupLayoutEntry = wgpu::Default;
		bindGroupLayoutEntry.binding = 0;
		bindGroupLayoutEntry.visibility = wgpu::ShaderStage::Vertex;
		bindGroupLayoutEntry.buffer.hasDynamicOffset = false;
		bindGroupLayoutEntry.buffer.type = wgpu::BufferBindingType::Uniform;
		bindGroupLayoutEntry.buffer.minBindingSize = sizeof(RenderGlobals::ShadowUniforms);

		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
		bindGroupLayoutDesc.label = { "shadowBindGroupLayout", WGPU_STRLEN };
		bindGroupLayoutDesc.entryCount = 1;
		bindGroupLayoutDesc.entries = &bindGroupLayoutEntry;
		wgpu::BindGroupLayout layout = GraphicsContext::GetDevice().createBindGroupLayout(bindGroupLayoutDesc);


		std::array<wgpu::BindGroupLayout, 2> bindGroupLayouts
		{
			layout,
			RenderGlobals::GetModelLayout()
		};	

		{
			wgpu::BindGroupEntry bindGroupEntry;
			bindGroupEntry.binding = 0;
			bindGroupEntry.buffer = RenderGlobals::GetShadowUniformBuffer();
			bindGroupEntry.offset = 0;
			bindGroupEntry.size = sizeof(RenderGlobals::ShadowUniforms);

			wgpu::BindGroupDescriptor bindGroupDesc{};
			bindGroupDesc.label = { "ShadowBindGroup", WGPU_STRLEN };
			bindGroupDesc.layout = layout;
			bindGroupDesc.entryCount = 1;
			bindGroupDesc.entries = &bindGroupEntry;
			m_BindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);
		}
	


		wgpu::PipelineLayoutDescriptor layoutDesc;
		layoutDesc.label = { "ShadowShaderPipelineLayout", WGPU_STRLEN };
		layoutDesc.bindGroupLayoutCount = bindGroupLayouts.size();
		layoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)&bindGroupLayouts;
		pipelineDesc.layout = GraphicsContext::GetDevice().createPipelineLayout(layoutDesc);

		m_ShadowPipeline = GraphicsContext::GetDevice().createRenderPipeline(pipelineDesc);

	}

	void ShadowPass::Execute(wgpu::CommandEncoder& encoder, const RenderContext& context, const DrawList& drawList)
	{
		wgpu::RenderPassDepthStencilAttachment depth;
		depth.view = m_DepthTextureView;
		depth.depthClearValue = 1.0f;
		depth.depthLoadOp = wgpu::LoadOp::Clear;
		depth.depthStoreOp = wgpu::StoreOp::Store;
		depth.depthReadOnly = wgpu::OptionalBool::False;
		depth.stencilClearValue = 0;
		depth.stencilLoadOp = wgpu::LoadOp::Undefined;
		depth.stencilStoreOp = wgpu::StoreOp::Undefined;
		depth.stencilReadOnly = wgpu::OptionalBool::True;

		wgpu::RenderPassDescriptor desc;
		desc.label = { "ShadowRenderPass", WGPU_STRLEN };
		desc.colorAttachmentCount = 0;
		desc.colorAttachments = nullptr;
		desc.depthStencilAttachment = &depth;

		wgpu::RenderPassEncoder pass = encoder.beginRenderPass(desc);

		pass.setPipeline(m_ShadowPipeline);

		for(const auto& lightEntitiy : context.scene->GetEntities<LightComponent>())
		{
			const auto& light = lightEntitiy.Get<LightComponent>();
			const auto& transform = lightEntitiy.Get<TransformComponent>();

			switch(light.type)
			{
			case LightType::Directional:
			{
				glm::quat q = glm::quat(glm::radians(transform.rotation));
				glm::vec3 forward = q * glm::vec3(0, 0, 1);

				glm::vec3 dir = glm::normalize(forward);
				glm::vec3 target = transform.position + dir;

				glm::vec3 up = q* glm::vec3(0, 1, 0);

				glm::mat4 view = glm::lookAtLH(transform.position, target, up);
				glm::mat4 proj = glm::orthoLH_ZO(-10.0f, 10.0f, -10.0f, 10.0f, -1000.0f, 1000.0f);

				s_ShadowUniforms.lightViewProj = proj * view;
				break;
			}
			case LightType::Spot:
			{
				break;
			}
			case LightType::Point:
			{
				break;
			}
			}
		}

		pass.setBindGroup(0, m_BindGroup, 0, nullptr);
		GraphicsContext::GetQueue().writeBuffer(RenderGlobals::GetShadowUniformBuffer(), 0, &s_ShadowUniforms, sizeof(s_ShadowUniforms));


		GraphicsContext::GetQueue().writeBuffer(RenderGlobals::GetShadowUniformBuffer(), 0, &s_ShadowUniforms, sizeof(s_ShadowUniforms));
		
		Mesh* mesh = nullptr;

		for (const DrawItem& draw : drawList.items)
		{
			if (draw.mesh == nullptr) continue;

			if (draw.mesh != mesh)
			{
				mesh = draw.mesh;
				pass.setVertexBuffer(0, mesh->GetVertexBuffer(), 0, mesh->GetVertexBuffer().getSize());
				pass.setIndexBuffer(mesh->GetIndexBuffer(), wgpu::IndexFormat::Uint32, 0, mesh->GetIndexBuffer().getSize());
			}
			const SubMesh* sub = draw.subMesh;

			uint32_t dynamicOffset = draw.modelIndex * RenderGlobals::GetModelUniformStride();
			pass.setBindGroup(1, RenderGlobals::GetModelBindGroup(), 1, &dynamicOffset);

			pass.drawIndexed(sub->indexCount, 1, sub->firstIndex, 0, 0);
		}

		pass.end();
	}

	wgpu::TextureView ShadowPass::GetShadowMap()
	{
		return m_DepthTextureView;
	}
	RenderGlobals::ShadowUniforms ShadowPass::GetShadowUniforms()
	{
		return s_ShadowUniforms;
	}
}