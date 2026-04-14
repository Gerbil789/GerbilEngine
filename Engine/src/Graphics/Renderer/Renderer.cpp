#include "enginepch.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Texture.h"
#include "Engine/Graphics/Renderer/DrawList.h"
#include "Engine/Graphics/RenderPass/ShadowPass.h"
#include "Engine/Scene/Components.h"
#include "Engine/Asset/Importer/TextureImporter.h"
#include <execution>
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
	struct alignas(16) ViewUniforms {
		glm::mat4 view;
		glm::mat4 projection;
		glm::vec3 cameraPosition;
		float padding;
	};
	static_assert(sizeof(ViewUniforms) % 16 == 0);

	struct alignas(16) ModelUniforms
	{
		glm::mat4 modelMatrix;
	};
	static_assert(sizeof(ModelUniforms) % 16 == 0);

	static wgpu::BindGroupLayout s_ViewBindGroupLayout = nullptr;
	static wgpu::BindGroupLayout s_ModelBindGroupLayout = nullptr;
	static wgpu::BindGroupLayout s_EnvironmentBindGroupLayout = nullptr;

	void Renderer::InitializeSharedResources()
	{
		CreateViewBindGroupLayout();
		CreateModelBindGroupLayout();
		CreateEnvironmentBindGroupLayout();
	}

	void Renderer::Initialize()
	{
		CreateViewUniformBuffer();
		CreateViewBindGroup();

		CreateModelUniformBuffer();
		CreateModelBindGroup();

		m_RenderContext.environmentCubemap = Engine::TextureImporter::LoadCubeMapTexture("Resources/Engine/hdr/PG2/lebombo_4k.hdr");
		CreateShadowTexture();

		CreateEnvironmentUniformBuffer();
		CreateEnvironmentBindGroup();
	}

	void Renderer::AddPass(RenderPass* pass)
	{
		m_Passes.push_back(pass);
	}

	void Renderer::RemovePass(RenderPass* pass)
	{
		auto it = std::find(m_Passes.begin(), m_Passes.end(), pass);
		if(it != m_Passes.end())
		{
			m_Passes.erase(it);
		}
	}

	void Renderer::SetCamera(Camera* camera)
	{
		m_RenderContext.camera = camera;
	}

	void Renderer::SetColorTarget(wgpu::TextureView colorView)
	{
		m_RenderContext.colorTarget = colorView;
	}

	void Renderer::SetDepthTarget(wgpu::TextureView depthView)
	{
		m_RenderContext.depthTarget = depthView;
	}

	void Renderer::CreateViewBindGroupLayout()
	{
		std::array<wgpu::BindGroupLayoutEntry, 1> entries;

		entries[0].binding = 0;
		entries[0].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
		entries[0].buffer.type = wgpu::BufferBindingType::Uniform;
		entries[0].buffer.minBindingSize = sizeof(ViewUniforms);

		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
		bindGroupLayoutDesc.label = { "ViewBindGroupLayout", WGPU_STRLEN };
		bindGroupLayoutDesc.entryCount = entries.size();
		bindGroupLayoutDesc.entries = entries.data();

		s_ViewBindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(bindGroupLayoutDesc);
	}

	void Renderer::CreateViewUniformBuffer()
	{
		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.label = { "ViewUniformBuffer", WGPU_STRLEN };
		bufferDesc.size = sizeof(ViewUniforms);
		bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
		m_RenderContext.viewUniformBuffer = GraphicsContext::GetDevice().createBuffer(bufferDesc);
	}


	void Renderer::CreateViewBindGroup()
	{
		std::array<wgpu::BindGroupEntry, 1> entries;

		entries[0].binding = 0;
		entries[0].buffer = m_RenderContext.viewUniformBuffer;
		entries[0].offset = 0;
		entries[0].size = sizeof(ViewUniforms);

		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.label = { "ViewBindGroup", WGPU_STRLEN };
		bindGroupDesc.layout = s_ViewBindGroupLayout;
		bindGroupDesc.entryCount = entries.size();
		bindGroupDesc.entries = entries.data();
		m_RenderContext.viewBindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);
	}

	void Renderer::CreateModelBindGroupLayout()
	{


		std::array<wgpu::BindGroupLayoutEntry, 1> entries;

		entries[0].binding = 0;
		entries[0].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
		entries[0].buffer.hasDynamicOffset = true;
		entries[0].buffer.type = wgpu::BufferBindingType::Uniform;
		entries[0].buffer.minBindingSize = sizeof(ModelUniforms);

		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
		bindGroupLayoutDesc.label = { "ModelBindGroupLayout", WGPU_STRLEN };
		bindGroupLayoutDesc.entryCount = entries.size();
		bindGroupLayoutDesc.entries = entries.data();
		s_ModelBindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(bindGroupLayoutDesc);
	}

	void Renderer::CreateModelUniformBuffer()
	{
		// max 1024 unique transforms per frame //TODO: make this configurable or dynamic
		const size_t bufferSize = 1024 * GraphicsContext::GetUniformBufferOffsetAlignment();

		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.label = { "ModelUniformBuffer", WGPU_STRLEN };
		bufferDesc.size = bufferSize;
		bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
		m_RenderContext.modelUniformBuffer = GraphicsContext::GetDevice().createBuffer(bufferDesc);
	}

	void Renderer::CreateModelBindGroup()
	{
		wgpu::BindGroupEntry bindGroupEntry;
		bindGroupEntry.binding = 0;
		bindGroupEntry.buffer = m_RenderContext.modelUniformBuffer;
		bindGroupEntry.offset = 0;
		bindGroupEntry.size = sizeof(ModelUniforms);

		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.label = { "ModelBindGroup", WGPU_STRLEN };
		bindGroupDesc.layout = s_ModelBindGroupLayout;
		bindGroupDesc.entryCount = 1;
		bindGroupDesc.entries = &bindGroupEntry;
		m_RenderContext.modelBindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);
	}

	void Renderer::CreateEnvironmentBindGroupLayout()
	{
		std::array<wgpu::BindGroupLayoutEntry, 7> entries;

		// Environment uniforms
		entries[0].binding = 0;
		entries[0].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
		entries[0].buffer.type = wgpu::BufferBindingType::Uniform;
		entries[0].buffer.minBindingSize = sizeof(EnvironmentUniforms);

		// Environment sampler
		entries[1].binding = 1;
		entries[1].visibility = wgpu::ShaderStage::Fragment;
		entries[1].sampler.type = wgpu::SamplerBindingType::Filtering;

		// BRDF integration texture
		entries[2].binding = 2;
		entries[2].visibility = wgpu::ShaderStage::Fragment;
		entries[2].texture.sampleType = wgpu::TextureSampleType::Float;
		entries[2].texture.viewDimension = wgpu::TextureViewDimension::_2D;
		entries[2].texture.multisampled = false;

		// Irradiance texture
		entries[3].binding = 3;
		entries[3].visibility = wgpu::ShaderStage::Fragment;
		entries[3].texture.sampleType = wgpu::TextureSampleType::Float;
		entries[3].texture.viewDimension = wgpu::TextureViewDimension::Cube;
		entries[3].texture.multisampled = false;

		// Prefiltered environment texture
		entries[4].binding = 4;
		entries[4].visibility = wgpu::ShaderStage::Fragment;
		entries[4].texture.sampleType = wgpu::TextureSampleType::Float;
		entries[4].texture.viewDimension = wgpu::TextureViewDimension::Cube;
		entries[4].texture.multisampled = false;

		// Shadow map sampler
		entries[5].binding = 5;
		entries[5].visibility = wgpu::ShaderStage::Fragment;
		entries[5].sampler.type = wgpu::SamplerBindingType::Comparison;

		// Shadow map texture
		entries[6].binding = 6;
		entries[6].visibility = wgpu::ShaderStage::Fragment;
		entries[6].texture.sampleType = wgpu::TextureSampleType::Depth;
		entries[6].texture.viewDimension = wgpu::TextureViewDimension::_2DArray;
		entries[6].texture.multisampled = false;


		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
		bindGroupLayoutDesc.label = { "EnvironmentBindGroupLayout", WGPU_STRLEN };
		bindGroupLayoutDesc.entryCount = entries.size();
		bindGroupLayoutDesc.entries = entries.data();

		s_EnvironmentBindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(bindGroupLayoutDesc);
	}

	void Renderer::CreateEnvironmentUniformBuffer()
	{
		wgpu::BufferDescriptor desc;
		desc.label = { "EnvironmentUniformBuffer", WGPU_STRLEN };
		desc.size = sizeof(EnvironmentUniforms);
		desc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
		m_RenderContext.environmentUniformBuffer = GraphicsContext::GetDevice().createBuffer(desc);
	}

	void Renderer::CreateEnvironmentBindGroup()
	{
		std::array<wgpu::BindGroupEntry, 7> entries;

		entries[0].binding = 0;
		entries[0].buffer = m_RenderContext.environmentUniformBuffer;
		entries[0].offset = 0;
		entries[0].size = sizeof(EnvironmentUniforms);

		wgpu::SamplerDescriptor envSamplerDesc;
		envSamplerDesc.label = { "EnvironmentSampler", WGPU_STRLEN };
		envSamplerDesc.minFilter = wgpu::FilterMode::Linear;
		envSamplerDesc.magFilter = wgpu::FilterMode::Linear;
		envSamplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
		envSamplerDesc.maxAnisotropy = 1;

		wgpu::Sampler envSampler = GraphicsContext::GetDevice().createSampler(envSamplerDesc);
		entries[1].binding = 1;
		entries[1].sampler = envSampler;

		auto brdfTexture = TextureImporter::LoadTexture2D("Resources/Engine/hdr/PG2/brdf_integration_map_ct_ggx.hdr");
		entries[2].binding = 2;
		entries[2].textureView = brdfTexture->GetTextureView();

		Engine::CubeMapTexture* irrCubemap = Engine::TextureImporter::LoadCubeMapTexture("Resources/Engine/hdr/PG2/lebombo_irradiance_map.hdr");
		entries[3].binding = 3;
		entries[3].textureView = irrCubemap->GetTextureView();

		entries[4].binding = 4;
		entries[4].textureView = m_RenderContext.environmentCubemap->GetTextureView();

		wgpu::SamplerDescriptor desc;
		desc.label = { "ShadowSampler", WGPU_STRLEN };
		desc.compare = wgpu::CompareFunction::LessEqual;
		desc.minFilter = wgpu::FilterMode::Linear;
		desc.magFilter = wgpu::FilterMode::Linear;
		desc.maxAnisotropy = 1;

		wgpu::Sampler shadowSampler = GraphicsContext::GetDevice().createSampler(desc);

		entries[5].binding = 5;
		entries[5].sampler = shadowSampler;

		entries[6].binding = 6;
		entries[6].textureView = m_RenderContext.depthTextureArrayView;


		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.label = { "EnvironmentBindGroup", WGPU_STRLEN };
		bindGroupDesc.layout = s_EnvironmentBindGroupLayout;
		bindGroupDesc.entryCount = entries.size();
		bindGroupDesc.entries = entries.data();
		m_RenderContext.environmentBindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);
	}

	void Renderer::CreateShadowTexture()
	{
		wgpu::TextureFormat format = wgpu::TextureFormat::Depth24Plus;

		wgpu::TextureDescriptor textureDesc;
		textureDesc.dimension = wgpu::TextureDimension::_2D;
		textureDesc.format = format;
		textureDesc.mipLevelCount = 1;
		textureDesc.sampleCount = 1;
		textureDesc.size = { 1024, 1024, s_ShadowCascadeCount };
		textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc;
		textureDesc.viewFormatCount = 0;
		textureDesc.viewFormats = nullptr;
		wgpu::Texture texture = GraphicsContext::GetDevice().createTexture(textureDesc);

		for (int i = 0; i < s_ShadowCascadeCount; i++)
		{
			wgpu::TextureViewDescriptor viewDesc;
			viewDesc.format = format;
			viewDesc.aspect = wgpu::TextureAspect::DepthOnly;
			viewDesc.dimension = wgpu::TextureViewDimension::_2D;
			viewDesc.baseMipLevel = 0;
			viewDesc.mipLevelCount = 1;
			viewDesc.baseArrayLayer = i;
			viewDesc.arrayLayerCount = 1;

			m_RenderContext.depthTextureViews[i] = texture.createView(viewDesc);
		}

		wgpu::TextureViewDescriptor arrayViewDesc;
		arrayViewDesc.format = format;
		arrayViewDesc.aspect = wgpu::TextureAspect::DepthOnly;
		arrayViewDesc.dimension = wgpu::TextureViewDimension::_2DArray;
		arrayViewDesc.baseMipLevel = 0;
		arrayViewDesc.mipLevelCount = 1;
		arrayViewDesc.baseArrayLayer = 0;
		arrayViewDesc.arrayLayerCount = s_ShadowCascadeCount;

		m_RenderContext.depthTextureArrayView = texture.createView(arrayViewDesc);
	}

	void Renderer::RenderScene(Scene* scene)
	{
		m_RenderContext.scene = scene;

		ViewUniforms viewUniforms;
		viewUniforms.view = m_RenderContext.camera->GetViewMatrix();
		viewUniforms.projection = m_RenderContext.camera->GetProjectionMatrix();
		viewUniforms.cameraPosition = m_RenderContext.camera->GetPosition();
		GraphicsContext::GetQueue().writeBuffer(m_RenderContext.viewUniformBuffer, 0, &viewUniforms, sizeof(viewUniforms));

		wgpu::CommandEncoder encoder = GraphicsContext::GetDevice().createCommandEncoder();

		m_RenderContext.drawList = DrawList::CreateFromScene(m_RenderContext.scene);

		std::vector<glm::mat4> models(m_RenderContext.drawList.size());

		std::for_each(std::execution::par, m_RenderContext.drawList.begin(), m_RenderContext.drawList.end(), [&](const DrawItem& item)
			{
				models[item.modelIndex] = item.entity.Get<TransformComponent>().GetWorldMatrix();
			});


		for (const DrawItem& item : m_RenderContext.drawList)
		{
			uint32_t offset = item.modelIndex * GraphicsContext::GetUniformBufferOffsetAlignment(); //TODO: cache the offset?
			GraphicsContext::GetQueue().writeBuffer(m_RenderContext.modelUniformBuffer, offset, &models[item.modelIndex], sizeof(glm::mat4));
		}

		for(auto pass : m_Passes)
		{
			pass->Execute(encoder, m_RenderContext);
		}

		wgpu::CommandBuffer commandBuffer = encoder.finish();
		GraphicsContext::GetQueue().submit(1, &commandBuffer);
	}

	wgpu::TextureView Renderer::GetTextureView() const
	{
		return m_RenderContext.colorTarget;
	}


	wgpu::BindGroupLayout Renderer::GetViewLayout()
	{
		return s_ViewBindGroupLayout;
	}

	wgpu::BindGroupLayout Renderer::GetModelLayout()
	{
		return s_ModelBindGroupLayout;
	}

	wgpu::BindGroupLayout Renderer::GetEnvironmentLayout()
	{
		return s_EnvironmentBindGroupLayout;
	}



}