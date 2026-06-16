#include "enginepch.h"
#include "Engine/Graphics/RenderPass/ShadowPass.h"
#include "Engine/Graphics/Renderer/RenderContext.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Graphics/WebGPUUtils.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Utility/File.h"
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"
#include "Engine/Asset/AssetManager.h"
#include <glm/gtx/quaternion.hpp>

namespace Engine
{

	float ShadowPass::s_Lambda = 0.9f;

	namespace 
	{
		wgpu::RenderPipeline m_ShadowPipeline;
		wgpu::BindGroup m_ShadowBindGroup;
		wgpu::Buffer m_ShadowUniformBuffer;
		std::array<glm::mat4, s_ShadowCascadeCount> m_LightViewProjMatrices;
	}

	Engine::ShadowPass::ShadowPass()
	{
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
		pipelineDesc.vertex.module = LoadWGSLShader("Resources/Engine/shaders/shadow.wgsl");
		pipelineDesc.vertex.entryPoint = { "vs_main", WGPU_STRLEN};
		pipelineDesc.vertex.constantCount = 0;
		pipelineDesc.vertex.constants = nullptr;

		pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
		pipelineDesc.primitive.frontFace = wgpu::FrontFace::CW;
		pipelineDesc.primitive.cullMode = wgpu::CullMode::Back;

		wgpu::DepthStencilState depthStencilState;
		depthStencilState.depthCompare = wgpu::CompareFunction::Less;
		depthStencilState.depthWriteEnabled = wgpu::OptionalBool::True;
		depthStencilState.format = wgpu::TextureFormat::Depth24Plus;
		depthStencilState.stencilReadMask = 0xFFFFFFFF;
		depthStencilState.stencilWriteMask = 0xFFFFFFFF;
		depthStencilState.depthBias = 10;
		depthStencilState.depthBiasSlopeScale = 3.0f;
		depthStencilState.depthBiasClamp = 50;

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
			RenderPipelineLayouts::GetModelLayout()
		};	

		{
			wgpu::BufferDescriptor shadowBufferDesc;
			shadowBufferDesc.label = { "ShadowPassUniformBuffer", WGPU_STRLEN };
			shadowBufferDesc.size = GraphicsContext::GetUniformBufferOffsetAlignment() * s_ShadowCascadeCount;
			shadowBufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
			m_ShadowUniformBuffer = GraphicsContext::GetDevice().createBuffer(shadowBufferDesc);

			wgpu::BindGroupEntry bindGroupEntry;
			bindGroupEntry.binding = 0;
			bindGroupEntry.buffer = m_ShadowUniformBuffer;
			bindGroupEntry.offset = 0;
			bindGroupEntry.size = sizeof(glm::mat4);

			wgpu::BindGroupDescriptor bindGroupDesc;
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

	void ShadowPass::Execute(wgpu::CommandEncoder& encoder, const RenderContext& context)
	{
		EnvironmentUniforms envUniforms;
		entt::registry& registry = context.scene->GetRegistry();
		auto entityView = registry.view<TransformComponent, LightComponent>();

		for(auto [entity, transform, light] : entityView.each())
		{
			switch (light.type)
			{
			case LightType::Directional:
			{
				std::vector<float> splits;
				splits.resize(s_ShadowCascadeCount);

				float near = context.camera->GetPerspectiveNear();
				float far = context.camera->GetPerspectiveFar();

				glm::quat q = glm::quat(glm::radians(transform.rotation));
				glm::vec3 forward = q * glm::vec3(0, 0, 1);
				glm::vec3 up = q * glm::vec3(0, 1, 0);

				for (int i = 0; i < s_ShadowCascadeCount; i++)
				{
					float p = (i + 1) / (float)s_ShadowCascadeCount;

					float log = near * std::pow(far / near, p);
					float lin = near + (far - near) * p;

					splits[i] = glm::mix(lin, log, s_Lambda);
				}

				for (int i = 0; i < s_ShadowCascadeCount; i++)
				{
					float prevSplit = (i == 0) ? near : splits[i - 1];
					float currSplit = splits[i];

					std::array<glm::vec3, 8> corners = context.camera->GetFrustumCornersWorld(prevSplit, currSplit);

					glm::vec3 center(0.0f);
					for (const auto& c : corners)
					{
						center += c;
					}

					center /= corners.size();

					glm::vec3 lightPos = center - forward * 100.0f;

					glm::mat4 view = glm::lookAtLH(lightPos, center, up);

					// Compute AABB
					glm::vec3 min(FLT_MAX);
					glm::vec3 max(-FLT_MAX);

					for (const auto& c : corners)
					{
						glm::vec3 v = glm::vec3(view * glm::vec4(c, 1.0f));
						min = glm::min(min, v);
						max = glm::max(max, v);
					}

					float extend = 200; // shadow distance

					min.z -= extend;
					max.z += extend;

					glm::mat4 proj = glm::orthoLH_ZO(min.x, max.x, min.y, max.y, min.z, max.z);
					m_LightViewProjMatrices[i] = proj * view;


					envUniforms.lightViewProj[i] = proj * view;
					envUniforms.cascadeSplits[i] = currSplit;
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

		for (int index = 0; index < s_ShadowCascadeCount; index++)
		{
			GraphicsContext::GetQueue().writeBuffer(m_ShadowUniformBuffer, index * GraphicsContext::GetUniformBufferOffsetAlignment(), &m_LightViewProjMatrices[index], sizeof(glm::mat4));
		}

		GraphicsContext::GetQueue().writeBuffer(context.environmentUniformBuffer, 0, &envUniforms, sizeof(EnvironmentUniforms));

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

		for (int index = 0; index < s_ShadowCascadeCount; index++)
		{
			depth.view = context.depthTextureViews[index];

			wgpu::RenderPassEncoder pass = encoder.beginRenderPass(desc);

			pass.setPipeline(m_ShadowPipeline);

			uint32_t offset = index * GraphicsContext::GetUniformBufferOffsetAlignment();
			pass.setBindGroup(0, m_ShadowBindGroup, 1, &offset);
			pass.setBindGroup(1, context.modelBindGroup, 0, nullptr);

			Uuid lastMeshId{};

			for (const auto& [i, item] : std::views::enumerate(context.drawList))
			{
				if (item.meshId != lastMeshId)
				{
					lastMeshId = item.meshId;
					const Engine::Mesh& mesh = Engine::AssetManager::GetAsset<Mesh>(item.meshId);
					pass.setVertexBuffer(0, mesh.GetVertexBuffer(), 0, mesh.GetVertexBuffer().getSize());
					pass.setIndexBuffer(mesh.GetIndexBuffer(), wgpu::IndexFormat::Uint32, 0, mesh.GetIndexBuffer().getSize());
				}

				pass.drawIndexed(item.indexCount, 1, item.firstIndex, 0, static_cast<uint32_t>(i));
			}

			pass.end();
		}
	}
}