#include "enginepch.h"
#include "Engine/Graphics/RenderPass/ShadowPass.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Utility/File.h"
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
	namespace 
	{
		wgpu::RenderPipeline m_ShadowPipeline;
		wgpu::BindGroup m_ShadowBindGroup;
		wgpu::Buffer m_ShadowUniformBuffer;
		std::array<glm::mat4, RenderGlobals::shadowCascadeCount> m_LightViewProjMatrices;
	}

	Engine::ShadowPass::ShadowPass()
	{
		const std::filesystem::path shaderPath = "Resources/Engine/shaders/shadow.wgsl";
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
		wgpu::ShaderModule shaderModule = GraphicsContext::GetDevice().createShaderModule(shaderDesc);


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
		bindGroupLayoutEntry.buffer.hasDynamicOffset = true;
		bindGroupLayoutEntry.buffer.type = wgpu::BufferBindingType::Uniform;
		bindGroupLayoutEntry.buffer.minBindingSize = sizeof(glm::mat4);


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
			wgpu::BufferDescriptor shadowBufferDesc;
			shadowBufferDesc.label = { "ShadowPassUniformBuffer", WGPU_STRLEN };
			shadowBufferDesc.size = RenderGlobals::UniformStride * RenderGlobals::shadowCascadeCount;
			shadowBufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
			m_ShadowUniformBuffer = GraphicsContext::GetDevice().createBuffer(shadowBufferDesc);


			wgpu::BindGroupEntry bindGroupEntry;
			bindGroupEntry.binding = 0;
			bindGroupEntry.buffer = m_ShadowUniformBuffer;
			bindGroupEntry.offset = 0;
			bindGroupEntry.size = sizeof(glm::mat4);

			wgpu::BindGroupDescriptor bindGroupDesc{};
			bindGroupDesc.label = { "ShadowBindGroup", WGPU_STRLEN };
			bindGroupDesc.layout = layout;
			bindGroupDesc.entryCount = 1;
			bindGroupDesc.entries = &bindGroupEntry;
			m_ShadowBindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);
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
		for (const auto& lightEntitiy : context.scene->GetEntities<LightComponent>())
		{
			const auto& light = lightEntitiy.Get<LightComponent>();
			const auto& transform = lightEntitiy.Get<TransformComponent>();

			switch (light.type)
			{
			case LightType::Directional:
			{

				std::vector<float> splits;
				splits.resize(RenderGlobals::shadowCascadeCount);

				float near = context.camera->Perspective().near;
				float far = context.camera->Perspective().far;
				float lambda = 0.9f; // 0 = linear, 1 = logarithmic

				for (int i = 0; i < RenderGlobals::shadowCascadeCount; i++)
				{
					float p = (i + 1) / (float)RenderGlobals::shadowCascadeCount;

					float log = near * std::pow(far / near, p);
					float lin = near + (far - near) * p;

					splits[i] = glm::mix(lin, log, lambda);
				}

				glm::quat q = glm::quat(glm::radians(transform.rotation));
				glm::vec3 forward = q * glm::vec3(0, 0, 1);
				glm::vec3 up = q * glm::vec3(0, 1, 0);

				for (int i = 0; i < RenderGlobals::shadowCascadeCount; i++)
				{
					float prevSplit = (i == 0) ? near : splits[i - 1];
					float currSplit = splits[i];

					std::array<glm::vec3, 8> corners = context.camera->GetFrustumCornersWorld(prevSplit, currSplit);

					glm::vec3 center(0.0f);
					for (auto& c : corners)
						center += c;
					center /= corners.size();

					glm::vec3 lightPos = center - forward * 100.0f;

					glm::mat4 view = glm::lookAtLH(lightPos, center, up);

					// Compute AABB
					glm::vec3 min(FLT_MAX);
					glm::vec3 max(-FLT_MAX);

					for (auto& c : corners)
					{
						glm::vec3 v = glm::vec3(view * glm::vec4(c, 1.0f));
						min = glm::min(min, v);
						max = glm::max(max, v);
					}


					// fix dissaperaing shadows of objects outside camera view

					float extend = 200.0f; // tune this

					min.z -= extend;
					max.z += extend;


					// Build ortho projection
					glm::mat4 proj = glm::orthoLH_ZO(min.x, max.x, min.y, max.y, min.z, max.z);
					m_LightViewProjMatrices[i] = proj * view;

					RenderGlobals::s_ShadowUniforms.lightViewProj[i] = proj * view;
					RenderGlobals::s_ShadowUniforms.cascadeSplits[i] = currSplit;
				}

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


		for (int i = 0; i < RenderGlobals::shadowCascadeCount; i++)
		{
			GraphicsContext::GetQueue().writeBuffer(m_ShadowUniformBuffer, i * RenderGlobals::UniformStride, &m_LightViewProjMatrices[i], sizeof(glm::mat4));
		}

		GraphicsContext::GetQueue().writeBuffer(RenderGlobals::GetShadowUniformBuffer(), 0, &RenderGlobals::s_ShadowUniforms, sizeof(RenderGlobals::s_ShadowUniforms));

		wgpu::RenderPassDepthStencilAttachment depth;
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

		for (int i = 0; i < RenderGlobals::shadowCascadeCount; i++)
		{
			depth.view = RenderGlobals::GetShadowTextureView(i);

			wgpu::RenderPassEncoder pass = encoder.beginRenderPass(desc);

			pass.setPipeline(m_ShadowPipeline);

			uint32_t offset = i * RenderGlobals::UniformStride;
			pass.setBindGroup(0, m_ShadowBindGroup, 1, &offset);


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

				uint32_t dynamicOffset = draw.modelIndex * RenderGlobals::UniformStride;
				pass.setBindGroup(1, RenderGlobals::GetModelBindGroup(), 1, &dynamicOffset);

				pass.drawIndexed(sub->indexCount, 1, sub->firstIndex, 0, 0);
			}

			pass.end();
		}
	}
}