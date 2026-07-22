#include "enginepch.h"
#include "Engine/Graphics/RenderPass/UIPass.h"
#include "Engine/Graphics/Pipeline.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Components.h"
#include "Engine/Graphics/WebGPUUtils.h"
#include "Engine/Core/Resources.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Project.h"
#include <glm/gtc/matrix_transform.hpp>
#include <entt/entt.hpp>
#include <map>
#include <glaze/glaze.hpp>

namespace Engine
{
	struct UIDrawItem
	{
		glm::vec4 rect;
		glm::vec4 color;
		glm::vec4 uvRect;
	};

	struct UIUniforms 
	{
		glm::mat4 ortho;
	};

	//static wgpu::RenderPipeline m_UIPipeline = nullptr;
	//static wgpu::BindGroupLayout s_UIBindGroupLayout = nullptr;

	//static wgpu::BindGroupLayout s_TextureBindGroupLayout = nullptr;
	//static wgpu::BindGroup uiBindGroup = nullptr;
	//static wgpu::Buffer uiUniformBuffer = nullptr;
	//static wgpu::Buffer uiStorageBuffer = nullptr;

	struct AtlasRegion 
	{
		int x, y, w, h;
	};

	struct UIConfig
	{
		uint64_t texture = 0;
		std::map<std::string, AtlasRegion> icons;
	};

	static UIConfig s_UIConfig;

	static Material s_UIMaterial;


	static void LoadUITextureAtlas()
	{
		const std::filesystem::path configPath = Project::GetActive().GetProjectDirectory() /  "ui.json";
		std::string buffer;

		if (auto ec = glz::read_file_json(s_UIConfig, configPath.string(), buffer))
		{
			LOG_ERROR("Failed to load ui config file '{}': {}", configPath.string(), glz::format_error(ec, buffer));
			return;
		}
	}


	static std::vector<UIDrawItem> GenerateUIDrawList(Scene* scene)
	{
		Uuid textureId = Uuid{ s_UIConfig.texture };
		const Texture2D& texture = AssetManager::GetAsset<Texture2D>(textureId);

		std::vector<UIDrawItem> drawList;

		entt::registry& registry = scene->GetRegistry();
		auto view = registry.view<UI::Rect>();

		for (auto entity : view)
		{
			const auto& rect = view.get<UI::Rect>(entity);
			UIDrawItem item;
			item.rect = glm::vec4{ rect.position, rect.size };
			item.color = rect.color;

			if(!rect.icon.empty())
			{
				if(s_UIConfig.icons.find(rect.icon) != s_UIConfig.icons.end())
				{
					const AtlasRegion& region = s_UIConfig.icons[rect.icon];
					item.uvRect = glm::vec4{ (float)region.x / texture.GetWidth(), (float)region.y / texture.GetHeight(), (float)region.w / texture.GetWidth(), (float)region.h / texture.GetHeight() };
				}
				else
				{
					item.uvRect = glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f };
				}
			}
			drawList.push_back(item);
		}

		return drawList;
	}

	//static void CreateUIBindGroupLayout()
	//{
	//	// UI
	//	{
	//		std::array<wgpu::BindGroupLayoutEntry, 2> entries;

	//		entries[0].binding = 0;
	//		entries[0].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
	//		entries[0].buffer.type = wgpu::BufferBindingType::Uniform;
	//		entries[0].buffer.minBindingSize = sizeof(UIUniforms);

	//		entries[1].binding = 1;
	//		entries[1].visibility = wgpu::ShaderStage::Vertex;
	//		entries[1].buffer.type = wgpu::BufferBindingType::ReadOnlyStorage;
	//		entries[1].buffer.minBindingSize = 0;

	//		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
	//		bindGroupLayoutDesc.label = { "UIBindGroupLayout", WGPU_STRLEN };
	//		bindGroupLayoutDesc.entryCount = entries.size();
	//		bindGroupLayoutDesc.entries = entries.data();

	//		s_UIBindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(bindGroupLayoutDesc);


	//	}

	//	// Texture
	//	{
	//		std::array<wgpu::BindGroupLayoutEntry, 2> entries;

	//		entries[0].binding = 0;
	//		entries[0].visibility = wgpu::ShaderStage::Fragment;
	//		entries[0].sampler.type = wgpu::SamplerBindingType::Filtering;
	//		entries[1].binding = 1;

	//		entries[1].visibility = wgpu::ShaderStage::Fragment;
	//		entries[1].texture.sampleType = wgpu::TextureSampleType::Float;
	//		entries[1].texture.viewDimension = wgpu::TextureViewDimension::_2D;
	//		entries[1].texture.multisampled = false;

	//		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
	//		bindGroupLayoutDesc.label = { "TextureBindGroupLayout", WGPU_STRLEN };
	//		bindGroupLayoutDesc.entryCount = entries.size();
	//		bindGroupLayoutDesc.entries = entries.data();
	//		s_TextureBindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(bindGroupLayoutDesc);
	//	}
	//}

	//static void CreateUIUniformBuffer()
	//{
	//	wgpu::BufferDescriptor bufferDesc;
	//	bufferDesc.label = { "UIUniformBuffer", WGPU_STRLEN };
	//	bufferDesc.size = sizeof(UIUniforms);
	//	bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
	//	uiUniformBuffer = GraphicsContext::GetDevice().createBuffer(bufferDesc);
	//}

	//static void CreateUIStorageBuffer()
	//{
	//	wgpu::BufferDescriptor bufferDesc;
	//	bufferDesc.label = { "UIStorageBuffer", WGPU_STRLEN };
	//	bufferDesc.size = sizeof(UIDrawItem) * 128;
	//	bufferDesc.usage = wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst;
	//	uiStorageBuffer = GraphicsContext::GetDevice().createBuffer(bufferDesc);
	//}

	//static void CreateUIBindGroup()
	//{
	//	std::array<wgpu::BindGroupEntry, 2> entries;

	//	entries[0].binding = 0;
	//	entries[0].buffer = uiUniformBuffer;
	//	entries[0].offset = 0;
	//	entries[0].size = sizeof(UIUniforms);

	//	entries[1].binding = 1;
	//	entries[1].buffer = uiStorageBuffer;
	//	entries[1].offset = 0;
	//	entries[1].size = sizeof(UIDrawItem) * 128;

	//	wgpu::BindGroupDescriptor bindGroupDesc;
	//	bindGroupDesc.label = { "UIBindGroup", WGPU_STRLEN };
	//	bindGroupDesc.layout = s_UIBindGroupLayout;
	//	bindGroupDesc.entryCount = entries.size();
	//	bindGroupDesc.entries = entries.data();
	//	uiBindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);
	//}

	//static void CreateUIPipeline()
	//{
	//	wgpu::ShaderModule shaderModule = LoadWGSLShader("Resources/Engine/shaders/UI.wgsl");

	//	wgpu::RenderPipelineDescriptor pipelineDesc;
	//	pipelineDesc.label = { "UIShaderPipeline", WGPU_STRLEN };

	//	pipelineDesc.vertex.bufferCount = 0;
	//	pipelineDesc.vertex.buffers = nullptr;
	//	pipelineDesc.vertex.module = shaderModule;
	//	pipelineDesc.vertex.entryPoint = { "vs_main", WGPU_STRLEN };
	//	pipelineDesc.vertex.constantCount = 0;
	//	pipelineDesc.vertex.constants = nullptr;

	//	pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
	//	pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
	//	pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
	//	pipelineDesc.primitive.cullMode = wgpu::CullMode::None;

	//	wgpu::ColorTargetState colorTarget;
	//	colorTarget.format = GraphicsContext::GetSurfaceFormat();
	//	colorTarget.writeMask = wgpu::ColorWriteMask::All;

	//	wgpu::FragmentState fragmentState;
	//	fragmentState.module = shaderModule;
	//	fragmentState.entryPoint = { "fs_main", WGPU_STRLEN };
	//	fragmentState.constantCount = 0;
	//	fragmentState.constants = nullptr;
	//	fragmentState.targetCount = 1;
	//	fragmentState.targets = &colorTarget;
	//	pipelineDesc.depthStencil = nullptr;
	//	pipelineDesc.fragment = &fragmentState;

	//	pipelineDesc.multisample.count = 1;
	//	pipelineDesc.multisample.mask = ~0u;
	//	pipelineDesc.multisample.alphaToCoverageEnabled = false;

	//	std::array<wgpu::BindGroupLayout, 1> bindGroupLayouts
	//	{
	//		s_UIBindGroupLayout
	//	};

	//	wgpu::PipelineLayoutDescriptor layoutDesc{};
	//	layoutDesc.label = { "UIShaderPipelineLayout", WGPU_STRLEN };
	//	layoutDesc.bindGroupLayoutCount = bindGroupLayouts.size();
	//	layoutDesc.bindGroupLayouts = reinterpret_cast<WGPUBindGroupLayout*>(bindGroupLayouts.data());
	//	pipelineDesc.layout = GraphicsContext::GetDevice().createPipelineLayout(layoutDesc);

	//	m_UIPipeline = GraphicsContext::GetDevice().createRenderPipeline(pipelineDesc);
	//}

	UIPass::UIPass()
	{
		LoadUITextureAtlas();

		const MaterialSpecification materialSpec
		{
			.shaderId = RESOURCES::SHADER::UI,
			.textures =
			{
				{ "uTexture", Uuid{ s_UIConfig.texture } }
			}
		};

		s_UIMaterial = Material(materialSpec);


		//CreateUIBindGroupLayout();
		//CreateUIUniformBuffer();
		//CreateUIStorageBuffer();
		//CreateUIBindGroup();
		//CreateUIPipeline();
	}

	void UIPass::Execute(wgpu::CommandEncoder& encoder, const RenderContext& context)
	{
		wgpu::RenderPassColorAttachment color;
		color.view = context.colorTarget;
		color.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		color.loadOp = wgpu::LoadOp::Load;
		color.storeOp = wgpu::StoreOp::Store;
		color.clearValue = wgpu::Color{ 0.0f, 0.0f, 0.0f, 0.0f };

		wgpu::RenderPassDescriptor passDescriptor;
		passDescriptor.label = { "UIRenderPass", WGPU_STRLEN };
		passDescriptor.colorAttachmentCount = 1;
		passDescriptor.colorAttachments = &color;
		passDescriptor.depthStencilAttachment = nullptr;

		wgpu::RenderPassEncoder pass = encoder.beginRenderPass(passDescriptor);


		wgpu::RenderPipeline pipeline = PipelineCache::GetOrCreatePipeline(s_UIMaterial.GetPipelineSpec());
		pass.setPipeline(pipeline);

		pass.setBindGroup(0, s_UIMaterial.GetBindGroup(), 0, nullptr);

		UIUniforms orthoUniform = { glm::ortho(0.0f, context.width, context.height, 0.0f,	-1.0f, 1.0f) };
		GraphicsContext::GetQueue().writeBuffer(s_UIMaterial.GetUniformBuffer(), 0, &orthoUniform, sizeof(UIUniforms));

		const std::vector<UIDrawItem> drawList = GenerateUIDrawList(context.scene);

		//std::map<Engine::Uuid, std::vector<UIDrawItem>> textureGroups;
		//for (auto& item : drawList) textureGroups[item.textureId].push_back(item);

		//// 2. Multi-draw
		//for (auto& [texId, items] : textureGroups) {
		//	// Upload items for this texture only
		//	GraphicsContext::GetQueue().writeBuffer(uiStorageBuffer, 0, items.data(), items.size() * sizeof(UIDrawItem));

		//	// Bind the specific texture for this draw call
		//	wgpu::BindGroup texBindGroup = GetBindGroupForTexture(texId);
		//	pass.setBindGroup(1, texBindGroup, 0, nullptr);

		//	pass.draw(6, (uint32_t)items.size(), 0, 0);
		//}









		//GraphicsContext::GetQueue().writeBuffer(s_UIMaterial.get, 0, drawList.data(), drawList.size() * sizeof(UIDrawItem));

		pass.draw(6, static_cast<uint32_t>(drawList.size()), 0, 0);

		pass.end();
	}
}