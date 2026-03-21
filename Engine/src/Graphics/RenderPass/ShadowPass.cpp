#include "enginepch.h"
#include "Engine/Graphics/RenderPass/ShadowPass.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"
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
		wgpu::Buffer m_UniformBuffer;
		static wgpu::TextureView m_DepthTextureView;

		struct ShadowUniforms
		{
			glm::mat4 lightViewProj;
		};
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
		textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::RenderAttachment;
		textureDesc.viewFormatCount = 0;
		textureDesc.viewFormats = nullptr;
		wgpu::Texture texture = GraphicsContext::GetDevice().createTexture(textureDesc);

		wgpu::TextureViewDescriptor viewDesc{};
		viewDesc.format = format;
		viewDesc.dimension = wgpu::TextureViewDimension::_2D;
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = 1;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 1;
		viewDesc.aspect = wgpu::TextureAspect::All;
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

		std::array<wgpu::VertexAttribute, 1> vertexAttribs;

		// Position
		vertexAttribs[0].shaderLocation = 0; // @location(0)
		vertexAttribs[0].format = wgpu::VertexFormat::Float32x3;
		vertexAttribs[0].offset = 0;

		wgpu::VertexBufferLayout vertexBufferLayout;

		vertexBufferLayout.attributeCount = static_cast<uint32_t>(vertexAttribs.size());
		vertexBufferLayout.attributes = vertexAttribs.data();
		vertexBufferLayout.arrayStride = 3 * sizeof(float);
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


		pipelineDesc.fragment = nullptr; // no fragment shader, no color output

		wgpu::DepthStencilState depthStencilState;
		depthStencilState.depthCompare = wgpu::CompareFunction::Less;
		depthStencilState.depthWriteEnabled = wgpu::OptionalBool::True;
		depthStencilState.format = wgpu::TextureFormat::Depth24Plus;
		depthStencilState.stencilReadMask = 0xFFFFFFFF;
		depthStencilState.stencilWriteMask = 0xFFFFFFFF;
		
		// I want to see how it looks when this is disabled, i will enable it later
		//depthStencilState.depthBias = 2;
		//depthStencilState.depthBiasSlopeScale = 2.0f;
		//depthStencilState.depthBiasClamp = 0.0f;

		pipelineDesc.depthStencil = &depthStencilState;

		pipelineDesc.multisample.count = 1;		// no multisampling
		pipelineDesc.multisample.mask = ~0u;	// all samples enabled
		pipelineDesc.multisample.alphaToCoverageEnabled = false;



		wgpu::BindGroupLayoutEntry bindGroupLayoutEntry = wgpu::Default;
		bindGroupLayoutEntry.binding = 0;
		bindGroupLayoutEntry.visibility = wgpu::ShaderStage::Vertex;
		bindGroupLayoutEntry.buffer.hasDynamicOffset = false;
		bindGroupLayoutEntry.buffer.type = wgpu::BufferBindingType::Uniform;
		bindGroupLayoutEntry.buffer.minBindingSize = sizeof(ShadowUniforms);

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

		// Crate buffer
		{
			wgpu::BufferDescriptor bufferDesc;
			bufferDesc.label = { "ShadowUniformBuffer", WGPU_STRLEN };
			bufferDesc.size = sizeof(ShadowUniforms);
			bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
			m_UniformBuffer = GraphicsContext::GetDevice().createBuffer(bufferDesc);

			wgpu::BindGroupEntry bindGroupEntry;
			bindGroupEntry.binding = 0;
			bindGroupEntry.buffer = m_UniformBuffer;
			bindGroupEntry.offset = 0;
			bindGroupEntry.size = sizeof(ShadowUniforms);

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
		depth.depthReadOnly = false;
		depth.stencilClearValue = 0;
		depth.stencilLoadOp = wgpu::LoadOp::Undefined;
		depth.stencilStoreOp = wgpu::StoreOp::Undefined;
		depth.stencilReadOnly = true;

		wgpu::RenderPassDescriptor passDescriptor;
		passDescriptor.label = { "ShadowRenderPass", WGPU_STRLEN };
		passDescriptor.colorAttachmentCount = 0;
		passDescriptor.colorAttachments = nullptr;
		passDescriptor.depthStencilAttachment = &depth;

		wgpu::RenderPassEncoder pass = encoder.beginRenderPass(passDescriptor);

		pass.setPipeline(m_ShadowPipeline);


		pass.setBindGroup(0, m_BindGroup, 0, nullptr);

		Scene* scene = context.scene;
		const auto& lights = scene->GetEntities<LightComponent>();

		if (lights.empty()) 
		{
			pass.end();
			return;
		}

		const Entity& lightEntity = lights[0];
		const auto& transform = lightEntity.Get<TransformComponent>();

		glm::vec3 forward = glm::normalize(glm::vec3(
			cos(glm::radians(transform.rotation.y)) * cos(glm::radians(transform.rotation.x)),
			sin(glm::radians(transform.rotation.x)),
			sin(glm::radians(transform.rotation.y)) * cos(glm::radians(transform.rotation.x))
		));

		glm::vec3 up = glm::vec3(0, 1, 0);


		glm::mat4 lightView = glm::lookAtLH(transform.position, transform.position + forward, up);

		glm::mat4 lightProj = glm::orthoLH(
			-20.0f, 20.0f,
			-20.0f, 20.0f,
			0.1f, 100.0f
		);

		ShadowUniforms uniforms;
		uniforms.lightViewProj = lightProj * lightView;

		GraphicsContext::GetQueue().writeBuffer(m_UniformBuffer, 0, &uniforms, sizeof(uniforms));
		
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
}