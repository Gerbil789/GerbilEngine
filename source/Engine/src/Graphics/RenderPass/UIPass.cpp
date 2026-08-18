#include "enginepch.h"
#include "Engine/Graphics/RenderPass/UIPass.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Graphics/SamplerPool.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Font.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/Components.h"
#include "Engine/Asset/Resources.h"
#include "Engine/Core/Project.h"
#include <glm/gtc/matrix_transform.hpp>
#include <entt/entt.hpp>
#include <map>
#include <glaze/glaze.hpp>

namespace Engine
{
	struct alignas(16) UIUniforms
	{
		glm::mat4 ortho;
	};
	static_assert(sizeof(UIUniforms) % 16 == 0);

	enum class UIDrawMode : uint32_t { Image = 0, Text = 1 };

	struct UIDrawItem
	{
		glm::vec4 rect;
		glm::vec4 color;
		glm::vec4 uvRect;
		UIDrawMode mode;
		glm::vec3 padding;
	};
	static_assert(sizeof(UIDrawItem) % 16 == 0);

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

	static wgpu::BindGroupLayout s_UIBindGroupLayout = nullptr;
	static wgpu::BindGroup uiBindGroup = nullptr;
	static wgpu::Buffer uiUniformBuffer = nullptr;
	static wgpu::Buffer uiStorageBuffer = nullptr;
	static wgpu::RenderPipeline uiPipeline = nullptr;

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

	static void CalculateUILayout(entt::registry& registry, entt::entity entity, const UI::RectTransform& parentRect, float scaleFactor, bool forceDirty)
	{
		bool isDirty = forceDirty || registry.any_of<UI::LayoutDirtyTag>(entity);

		if (isDirty)
		{
			auto& rect = registry.get<UI::RectTransform>(entity);

			glm::vec2 scaledSize = rect.size * scaleFactor;
			glm::vec2 scaledAnchoredPos = rect.anchoredPosition * scaleFactor;

			float baseX = parentRect.absolutePosition.x + (parentRect.absoluteSize.x * rect.anchorMin.x);
			float baseY = parentRect.absolutePosition.y + (parentRect.absoluteSize.y * rect.anchorMin.y);

			float width = parentRect.absoluteSize.x * (rect.anchorMax.x - rect.anchorMin.x) + scaledSize.x;
			float height = parentRect.absoluteSize.y * (rect.anchorMax.y - rect.anchorMin.y) + scaledSize.y;

			float finalX = baseX + scaledAnchoredPos.x - (width * rect.pivot.x);
			float finalY = baseY + scaledAnchoredPos.y - (height * rect.pivot.y);

			rect.absolutePosition = { finalX, finalY };
			rect.absoluteSize = { width, height };
			registry.remove<UI::LayoutDirtyTag>(entity);
		}

		const auto& hc = registry.get<HierarchyComponent>(entity);
		const auto& rect = registry.get<UI::RectTransform>(entity);

		for (entt::entity child : hc.children)
		{
			if (registry.any_of<UI::RectTransform>(child))
			{
				CalculateUILayout(registry, child, rect, scaleFactor, isDirty);
			}
		}
	}

	static void CollectUIDrawItems(entt::registry& registry, entt::entity entity, std::vector<UIDrawItem>& drawList, float texWidth, float texHeight)
	{
		if (registry.any_of<DisabledTag>(entity)) return;

		if (registry.any_of<UI::RectTransform>(entity))
		{
			const auto& rect = registry.get<UI::RectTransform>(entity);

			if (registry.any_of<UI::Image>(entity))
			{
				const auto& image = registry.get<UI::Image>(entity);
				UIDrawItem item;
				item.rect = glm::vec4(rect.absolutePosition.x, rect.absolutePosition.y, rect.absoluteSize.x, rect.absoluteSize.y);
				item.color = image.tint;
				item.mode = UIDrawMode::Image;

				if (!image.iconName.empty() && s_UIConfig.icons.find(image.iconName) != s_UIConfig.icons.end())
				{
					const AtlasRegion& region = s_UIConfig.icons[image.iconName];
					item.uvRect = glm::vec4(
						static_cast<float>(region.x) / texWidth,
						static_cast<float>(region.y) / texHeight,
						static_cast<float>(region.w) / texWidth,
						static_cast<float>(region.h) / texHeight
					);
				}
				else
				{
					constexpr glm::vec4 defaultUV(0.0f, 0.0f, 48.0f / 2048.0f, 48.0f / 2048.0f);
					item.uvRect = defaultUV;
				}

				drawList.push_back(item);
			}

			if (registry.any_of<UI::Text>(entity))
			{
				const auto& text = registry.get<UI::Text>(entity);
				const Font* font = FontManager::GetFont(text.fontName);

				if (font)
				{
					float cursorX = rect.absolutePosition.x;
					float cursorY = rect.absolutePosition.y + text.fontSize;

					float maxX = rect.absolutePosition.x + rect.absoluteSize.x;

					for (size_t i = 0; i < text.text.length(); ++i)
					{
						char c = text.text[i];

						if (c == '\n')
						{
							cursorX = rect.absolutePosition.x;
							cursorY += text.fontSize * text.lineSpacing;
							continue;
						}

						if (text.wrapText && c != ' ')
						{
							bool isStartOfWord = (i == 0 || text.text[i - 1] == ' ' || text.text[i - 1] == '\n');

							if (isStartOfWord)
							{
								float wordWidth = 0.0f;
								size_t j = i;

								while (j < text.text.length() && text.text[j] != ' ' && text.text[j] != '\n')
								{
									Glyph* lookaheadGlyph = font->GetGlyph(static_cast<uint32_t>(text.text[j]));
									if (lookaheadGlyph)
									{
										wordWidth += lookaheadGlyph->advance * text.fontSize;
									}
									j++;
								}

								if (cursorX + wordWidth > maxX && cursorX > rect.absolutePosition.x)
								{
									cursorX = rect.absolutePosition.x;
									cursorY += text.fontSize * text.lineSpacing;
								}
							}
						}

						if (c == ' ' && cursorX == rect.absolutePosition.x)
						{
							continue;
						}

						uint32_t unicode = static_cast<uint32_t>(c);
						Glyph* glyph = font->GetGlyph(unicode);

						if (!glyph)
						{
							LOG_WARNING("Glyph for character '{}' (unicode {}) not found in font '{}'", c, unicode, text.fontName);
							continue;
						}

						float x = cursorX + (glyph->planeBounds.left * text.fontSize);
						float y = cursorY - (glyph->planeBounds.top * text.fontSize);
						float w = (glyph->planeBounds.right - glyph->planeBounds.left) * text.fontSize;
						float h = (glyph->planeBounds.top - glyph->planeBounds.bottom) * text.fontSize;

						if (w > 0.0f && h > 0.0f && c != ' ')
						{
							UIDrawItem item;
							item.rect = glm::vec4(x, y, w, h);
							item.color = text.color;
							item.mode = UIDrawMode::Text;

							constexpr float atlasSize = 1024.0f;
							float uLeft = glyph->atlasBounds.left / atlasSize;
							float uRight = glyph->atlasBounds.right / atlasSize;
							float uTop = 1.0f - (glyph->atlasBounds.top / atlasSize);
							float uBottom = 1.0f - (glyph->atlasBounds.bottom / atlasSize);

							item.uvRect = glm::vec4(uLeft, uTop, (uRight - uLeft), (uBottom - uTop));
							drawList.push_back(item);
						}

						cursorX += glyph->advance * text.fontSize;
					}
				}
			}

		}

		if (registry.any_of<HierarchyComponent>(entity))
		{
			const auto& hc = registry.get<HierarchyComponent>(entity);
			for (entt::entity child : hc.children)
			{
				CollectUIDrawItems(registry, child, drawList, texWidth, texHeight);
			}
		}
	}

	static std::vector<UIDrawItem> GenerateUIDrawList(Scene* scene, float screenWidth, float screenHeight)
	{
		entt::registry& registry = scene->GetRegistry();
		std::vector<UIDrawItem> drawList;

		//TODO: use on window resize event...

		static float lastScreenWidth = 0.0f;
		static float lastScreenHeight = 0.0f;
		bool screenResized = (screenWidth != lastScreenWidth || screenHeight != lastScreenHeight);

		lastScreenWidth = screenWidth;
		lastScreenHeight = screenHeight;

		bool anyLayoutDirty = screenResized || !registry.view<UI::LayoutDirtyTag>().empty();

		if (anyLayoutDirty)
		{
			auto view = registry.view<UI::Canvas, UI::RectTransform, HierarchyComponent>(entt::exclude<DisabledTag>);
			for (auto [entity, canvas, rect, hc] : view.each())
			{
				if (canvas.isScreenSpace)
				{
					bool canvasDirty = screenResized || registry.any_of<UI::LayoutDirtyTag>(entity);

					if (canvasDirty)
					{
						rect.anchoredPosition = { 0.0f, 0.0f };
						rect.size = { screenWidth, screenHeight };
						rect.absolutePosition = { 0.0f, 0.0f };
						rect.absoluteSize = { screenWidth, screenHeight };
						registry.remove<UI::LayoutDirtyTag>(entity);
					}

					float scaleX = screenWidth / canvas.referenceResolution.x;
					float scaleY = screenHeight / canvas.referenceResolution.y;
					float scaleFactor = std::lerp(scaleX, scaleY, canvas.matchWidthOrHeight);

					for (entt::entity child : hc.children)
					{
						if (registry.any_of<UI::RectTransform>(child))
						{
							CalculateUILayout(registry, child, rect, scaleFactor, canvasDirty);
						}
					}
				}
				else
				{
					//TODO
				}
			}
		}


		const Texture2D& texture = AssetManager::GetAsset<Texture2D>(Uuid{ s_UIConfig.texture });
		float texWidth = static_cast<float>(texture.GetWidth());
		float texHeight = static_cast<float>(texture.GetHeight());

		auto canvasView = registry.view<UI::Canvas, HierarchyComponent>(entt::exclude<DisabledTag>);
		for (auto [entity, canvas, hc] : canvasView.each())
		{
			if (canvas.isScreenSpace)
			{
				for (entt::entity child : hc.children)
				{
					CollectUIDrawItems(registry, child, drawList, texWidth, texHeight);
				}
			}
		}

		return drawList;
	}

	static void CreateUIUniformBuffer()
	{
		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.label = "UIUniformBuffer";
		bufferDesc.size = sizeof(UIUniforms);
		bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
		uiUniformBuffer = GraphicsContext::GetDevice().CreateBuffer(&bufferDesc);
	}

	static void CreateUIStorageBuffer()
	{
		constexpr uint64_t maxUIElements = 16384;

		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.label = "UIStorageBuffer";
		bufferDesc.size = sizeof(UIDrawItem) * maxUIElements;
		bufferDesc.usage = wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst;
		uiStorageBuffer = GraphicsContext::GetDevice().CreateBuffer(&bufferDesc);
	}

	void CreateUIBindGroupLayout()
	{
		std::array<wgpu::BindGroupLayoutEntry, 6> entries;

		entries[0].binding = 0;
		entries[0].visibility = wgpu::ShaderStage::Vertex;
		entries[0].buffer.type = wgpu::BufferBindingType::Uniform;
		entries[0].buffer.minBindingSize = sizeof(UIUniforms);

		entries[1].binding = 1;
		entries[1].visibility = wgpu::ShaderStage::Vertex;
		entries[1].buffer.type = wgpu::BufferBindingType::ReadOnlyStorage;
		entries[1].buffer.minBindingSize = sizeof(UIDrawItem);

		entries[2].binding = 2;
		entries[2].visibility = wgpu::ShaderStage::Fragment;
		entries[2].texture.sampleType = wgpu::TextureSampleType::Float;
		entries[2].texture.multisampled = false;
		entries[2].texture.viewDimension = wgpu::TextureViewDimension::e2D;

		entries[3].binding = 3;
		entries[3].visibility = wgpu::ShaderStage::Fragment;
		entries[3].sampler.type = wgpu::SamplerBindingType::Filtering;

		entries[4].binding = 4;
		entries[4].visibility = wgpu::ShaderStage::Fragment;
		entries[4].texture.sampleType = wgpu::TextureSampleType::Float;
		entries[4].texture.multisampled = false;
		entries[4].texture.viewDimension = wgpu::TextureViewDimension::e2D;

		entries[5].binding = 5;
		entries[5].visibility = wgpu::ShaderStage::Fragment;
		entries[5].sampler.type = wgpu::SamplerBindingType::Filtering;

		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
		bindGroupLayoutDesc.label = "UIBindGroupLayout";
		bindGroupLayoutDesc.entryCount = entries.size();
		bindGroupLayoutDesc.entries = entries.data();

		s_UIBindGroupLayout = GraphicsContext::GetDevice().CreateBindGroupLayout(&bindGroupLayoutDesc);
	}

	static void CreateUIBindGroup()
	{
		std::array<wgpu::BindGroupEntry, 6> entries;

		entries[0].binding = 0;
		entries[0].buffer = uiUniformBuffer;
		entries[0].offset = 0;
		entries[0].size = uiUniformBuffer.GetSize();
		
		entries[1].binding = 1;
		entries[1].buffer = uiStorageBuffer;
		entries[1].offset = 0;
		entries[1].size = uiStorageBuffer.GetSize();

		entries[2].binding = 2;
		entries[2].textureView = AssetManager::GetAsset<Texture2D>(Uuid{ s_UIConfig.texture }).GetTextureView();

		entries[3].binding = 3;
		entries[3].sampler = SamplerPool::GetSampler({ TextureFilter::Point, TextureWrap::Repeat });

		entries[4].binding = 4;
		entries[4].textureView = AssetManager::GetAsset<Texture2D>(RESOURCES::TEXTURE::DEFAULT_FONT_ATLAS).GetTextureView();

		entries[5].binding = 5;
		entries[5].sampler = SamplerPool::GetSampler({ TextureFilter::Bilinear, TextureWrap::Clamp });

		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.label = "UIBindGroup";
		bindGroupDesc.layout = s_UIBindGroupLayout;
		bindGroupDesc.entryCount = entries.size();
		bindGroupDesc.entries = entries.data();
		uiBindGroup = GraphicsContext::GetDevice().CreateBindGroup(&bindGroupDesc);
	}

	void CreateUIPipeline()
	{
		const Shader& shader = AssetManager::GetAsset<Shader>(RESOURCES::SHADER::UI);

		wgpu::RenderPipelineDescriptor pipelineDesc;
		pipelineDesc.label = "UI Shader Pipeline";

		pipelineDesc.vertex.bufferCount = 0;
		pipelineDesc.vertex.buffers = nullptr;
		pipelineDesc.vertex.module = shader.GetShaderModule();
		pipelineDesc.vertex.entryPoint = "vs_main";

		pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
		pipelineDesc.primitive.frontFace = wgpu::FrontFace::CW;
		pipelineDesc.primitive.cullMode = wgpu::CullMode::Back;

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
		fragmentState.module = shader.GetShaderModule();
		fragmentState.entryPoint = "fs_main";
		fragmentState.constantCount = 0;
		fragmentState.constants = nullptr;
		fragmentState.targetCount = 1;
		fragmentState.targets = &colorTarget;
		pipelineDesc.fragment = &fragmentState;

		pipelineDesc.depthStencil = nullptr;
		pipelineDesc.multisample.count = 1;
		pipelineDesc.multisample.mask = ~0u;

		std::array<wgpu::BindGroupLayout, 1> bindGroupLayouts;
		bindGroupLayouts[0] = s_UIBindGroupLayout;

		wgpu::PipelineLayoutDescriptor layoutDesc;
		layoutDesc.label = "UI Shader Pipeline Layout";
		layoutDesc.bindGroupLayoutCount = bindGroupLayouts.size();
		layoutDesc.bindGroupLayouts = bindGroupLayouts.data();
		pipelineDesc.layout = GraphicsContext::GetDevice().CreatePipelineLayout(&layoutDesc);

		uiPipeline = GraphicsContext::GetDevice().CreateRenderPipeline(&pipelineDesc);
	}

	UIPass::UIPass()
	{
		LoadUITextureAtlas();
		CreateUIBindGroupLayout();
		CreateUIPipeline();
		CreateUIUniformBuffer();
		CreateUIStorageBuffer();
		CreateUIBindGroup();
	}

	void UIPass::Execute(wgpu::CommandEncoder& encoder, const RenderContext& context)
	{
		wgpu::RenderPassColorAttachment color;
		color.view = context.colorTarget;
		color.loadOp = wgpu::LoadOp::Load;
		color.storeOp = wgpu::StoreOp::Store;
		color.clearValue = wgpu::Color{ 0.0f, 0.0f, 0.0f, 0.0f };

		wgpu::RenderPassDescriptor passDescriptor;
		passDescriptor.label = "UIRenderPass";
		passDescriptor.colorAttachmentCount = 1;
		passDescriptor.colorAttachments = &color;
		passDescriptor.depthStencilAttachment = nullptr;

		wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&passDescriptor);

		pass.SetPipeline(uiPipeline);
		pass.SetBindGroup(0, uiBindGroup, 0, nullptr);

		const UIUniforms orthoUniform = { glm::ortho(0.0f, context.width, context.height, 0.0f,	-1.0f, 1.0f) };
		GraphicsContext::GetQueue().WriteBuffer(uiUniformBuffer, 0, &orthoUniform, sizeof(UIUniforms));

		const std::vector<UIDrawItem> drawList = GenerateUIDrawList(context.scene, context.width, context.height);
		GraphicsContext::GetQueue().WriteBuffer(uiStorageBuffer, 0, drawList.data(), drawList.size() * sizeof(UIDrawItem));

		pass.Draw(6, static_cast<uint32_t>(drawList.size()), 0, 0);
		pass.End();
	}
}