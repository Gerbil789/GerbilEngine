#include "enginepch.h"
//#include "engine/graphics/Renderpass/uipass.h"
//#include "engine/graphics/graphics_context.h"
//#include "engine/graphics/shader.h"
//#include "engine/graphics/sampler_pool.h"
//#include "engine/graphics/texture/texture_2d.h"
//#include "engine/graphics/font.h"
//#include "engine/asset/asset_manager.h"
//#include "engine/core/scene.h"
//#include "engine/core/components.h"
//#include "engine/asset/Resources.h"
//#include "engine/core/project.h"
//#include "engine/asset/asset_manager.h"
//#include "engine/core/SceneManager.h"
//#include <glm/gtc/matrix_transform.hpp>
//#include <entt/entt.hpp>
//#include <map>
//#include <glaze/glaze.hpp>
//
//namespace engine
//{
//	struct alignas(16) uiUniforms
//	{
//		glm::mat4 ortho;
//	};
//	static_assert(sizeof(uiUniforms) % 16 == 0);
//
//	enum class uiDrawMode : uint32_t { Image = 0, Text = 1 };
//
//	struct uiDrawItem
//	{
//		glm::vec4 rect;
//		glm::vec4 color;
//		glm::vec4 uvRect;
//		uiDrawMode mode;
//		glm::vec3 padding;
//	};
//	static_assert(sizeof(uiDrawItem) % 16 == 0);
//
//	struct AtlasRegion 
//	{
//		int x, y, w, h;
//	};
//
//	struct uiConfig
//	{
//		uint64_t texture = 0; //TODO: use texture2D
//		std::map<std::string, AtlasRegion> icons;
//	};
//
//	static uiConfig s_uIConfig;
//
//	static wgpu::BindGroupLayout s_uIBindGroupLayout = nullptr;
//	static wgpu::BindGroup uiBindGroup = nullptr;
//	static wgpu::Buffer uiUniformBuffer = nullptr;
//	static wgpu::Buffer uiStorageBuffer = nullptr;
//	static wgpu::RenderPipeline uiPipeline = nullptr;
//
//	static void LoaduitextureAtlas()
//	{
//		const std::filesystem::path configPath = engine::Project::Directory() /  "ui.json";
//		std::string buffer;
//
//		if (auto ec = glz::read_file_json(s_uIConfig, configPath.string(), buffer))
//		{
//			LOG_ERROR("Failed to load ui config file '{}': {}", configPath, glz::format_error(ec, buffer));
//			return;
//		}
//	}
//
//	static void CalculateuiLayout(entt::registry& registry, entt::entity entity, const ui::RectTransform& parentRect, float scaleFactor, bool forceDirty)
//	{
//		bool isDirty = forceDirty || registry.any_of<ui::LayoutDirtyTag>(entity);
//
//		if (isDirty)
//		{
//			auto& rect = registry.get<ui::RectTransform>(entity);
//
//			glm::vec2 scaledSize = rect.size * scaleFactor;
//			glm::vec2 scaledAnchoredPos = rect.anchoredPosition * scaleFactor;
//
//			float baseX = parentRect.absolutePosition.x + (parentRect.absoluteSize.x * rect.anchorMin.x);
//			float baseY = parentRect.absolutePosition.y + (parentRect.absoluteSize.y * rect.anchorMin.y);
//
//			float width = parentRect.absoluteSize.x * (rect.anchorMax.x - rect.anchorMin.x) + scaledSize.x;
//			float height = parentRect.absoluteSize.y * (rect.anchorMax.y - rect.anchorMin.y) + scaledSize.y;
//
//			float finalX = baseX + scaledAnchoredPos.x - (width * rect.pivot.x);
//			float finalY = baseY + scaledAnchoredPos.y - (height * rect.pivot.y);
//
//			rect.absolutePosition = { finalX, finalY };
//			rect.absoluteSize = { width, height };
//			registry.remove<ui::LayoutDirtyTag>(entity);
//		}
//
//		const auto& hc = registry.get<HierarchyComponent>(entity);
//		const auto& rect = registry.get<ui::RectTransform>(entity);
//
//		for (entt::entity child : hc.children)
//		{
//			if (registry.any_of<ui::RectTransform>(child))
//			{
//				CalculateuiLayout(registry, child, rect, scaleFactor, isDirty);
//			}
//		}
//	}
//
//	static void CollectuiDrawItems(entt::registry& registry, entt::entity entity, std::vector<uiDrawItem>& drawList, float texWidth, float texHeight)
//	{
//		if (registry.any_of<DisabledTag>(entity)) return;
//
//		if (registry.any_of<ui::RectTransform>(entity))
//		{
//			const auto& rect = registry.get<ui::RectTransform>(entity);
//
//			if (registry.any_of<ui::Image>(entity))
//			{
//				const auto& image = registry.get<ui::Image>(entity);
//				uiDrawItem item;
//				item.rect = glm::vec4(rect.absolutePosition.x, rect.absolutePosition.y, rect.absoluteSize.x, rect.absoluteSize.y);
//				item.color = image.tint;
//				item.mode = uiDrawMode::Image;
//
//				if (!image.iconName.empty() && s_uIConfig.icons.find(image.iconName) != s_uIConfig.icons.end())
//				{
//					const AtlasRegion& region = s_uIConfig.icons[image.iconName];
//					item.uvRect = glm::vec4(
//						static_cast<float>(region.x) / texWidth,
//						static_cast<float>(region.y) / texHeight,
//						static_cast<float>(region.w) / texWidth,
//						static_cast<float>(region.h) / texHeight
//					);
//				}
//				else
//				{
//					constexpr glm::vec4 defaultUV(0.0f, 0.0f, 48.0f / 2048.0f, 48.0f / 2048.0f);
//					item.uvRect = defaultUV;
//				}
//
//				drawList.push_back(item);
//			}
//
//			if (registry.any_of<ui::Text>(entity))
//			{
//				const auto& text = registry.get<ui::Text>(entity);
//				const Font* font = FontManager::GetFont(text.fontName);
//
//				if (font)
//				{
//					float cursorX = rect.absolutePosition.x;
//					float cursorY = rect.absolutePosition.y + text.fontSize;
//
//					float maxX = rect.absolutePosition.x + rect.absoluteSize.x;
//
//					for (size_t i = 0; i < text.text.length(); ++i)
//					{
//						char c = text.text[i];
//
//						if (c == '\n')
//						{
//							cursorX = rect.absolutePosition.x;
//							cursorY += text.fontSize * text.lineSpacing;
//							continue;
//						}
//
//						if (text.wrapText && c != ' ')
//						{
//							bool isStartOfWord = (i == 0 || text.text[i - 1] == ' ' || text.text[i - 1] == '\n');
//
//							if (isStartOfWord)
//							{
//								float wordWidth = 0.0f;
//								size_t j = i;
//
//								while (j < text.text.length() && text.text[j] != ' ' && text.text[j] != '\n')
//								{
//									Glyph* lookaheadGlyph = font->GetGlyph(static_cast<uint32_t>(text.text[j]));
//									if (lookaheadGlyph)
//									{
//										wordWidth += lookaheadGlyph->advance * text.fontSize;
//									}
//									j++;
//								}
//
//								if (cursorX + wordWidth > maxX && cursorX > rect.absolutePosition.x)
//								{
//									cursorX = rect.absolutePosition.x;
//									cursorY += text.fontSize * text.lineSpacing;
//								}
//							}
//						}
//
//						if (c == ' ' && cursorX == rect.absolutePosition.x)
//						{
//							continue;
//						}
//
//						uint32_t unicode = static_cast<uint32_t>(c);
//						Glyph* glyph = font->GetGlyph(unicode);
//
//						if (!glyph)
//						{
//							LOG_WARNING("Glyph for character '{}' (unicode {}) not found in font '{}'", c, unicode, text.fontName);
//							continue;
//						}
//
//						float x = cursorX + (glyph->planeBounds.left * text.fontSize);
//						float y = cursorY - (glyph->planeBounds.top * text.fontSize);
//						float w = (glyph->planeBounds.right - glyph->planeBounds.left) * text.fontSize;
//						float h = (glyph->planeBounds.top - glyph->planeBounds.bottom) * text.fontSize;
//
//						if (w > 0.0f && h > 0.0f && c != ' ')
//						{
//							uiDrawItem item;
//							item.rect = glm::vec4(x, y, w, h);
//							item.color = text.color;
//							item.mode = uiDrawMode::Text;
//
//							constexpr float atlasSize = 1024.0f;
//							float uLeft = glyph->atlasBounds.left / atlasSize;
//							float uRight = glyph->atlasBounds.right / atlasSize;
//							float uTop = 1.0f - (glyph->atlasBounds.top / atlasSize);
//							float uBottom = 1.0f - (glyph->atlasBounds.bottom / atlasSize);
//
//							item.uvRect = glm::vec4(uLeft, uTop, (uRight - uLeft), (uBottom - uTop));
//							drawList.push_back(item);
//						}
//
//						cursorX += glyph->advance * text.fontSize;
//					}
//				}
//			}
//
//		}
//
//		if (registry.any_of<HierarchyComponent>(entity))
//		{
//			const auto& hc = registry.get<HierarchyComponent>(entity);
//			for (entt::entity child : hc.children)
//			{
//				CollectuiDrawItems(registry, child, drawList, texWidth, texHeight);
//			}
//		}
//	}
//
//	static std::vector<uiDrawItem> GenerateuiDrawList(Scene scene, float screenWidth, float screenHeight)
//	{
//		entt::registry& registry = engine::assetManager::Getasset(scene).GetRegistry();
//		std::vector<uiDrawItem> drawList;
//
//		//TODO: use on window resize event...
//
//		static float lastScreenWidth = 0.0f;
//		static float lastScreenHeight = 0.0f;
//		bool screenResized = (screenWidth != lastScreenWidth || screenHeight != lastScreenHeight);
//
//		lastScreenWidth = screenWidth;
//		lastScreenHeight = screenHeight;
//
//		bool anyLayoutDirty = screenResized || !registry.view<ui::LayoutDirtyTag>().empty();
//
//		if (anyLayoutDirty)
//		{
//			auto view = registry.view<ui::Canvas, ui::RectTransform, HierarchyComponent>(entt::exclude<DisabledTag>);
//			for (auto [entity, canvas, rect, hc] : view.each())
//			{
//				if (canvas.isScreenSpace)
//				{
//					bool canvasDirty = screenResized || registry.any_of<ui::LayoutDirtyTag>(entity);
//
//					if (canvasDirty)
//					{
//						rect.anchoredPosition = { 0.0f, 0.0f };
//						rect.size = { screenWidth, screenHeight };
//						rect.absolutePosition = { 0.0f, 0.0f };
//						rect.absoluteSize = { screenWidth, screenHeight };
//						registry.remove<ui::LayoutDirtyTag>(entity);
//					}
//
//					float scaleX = screenWidth / canvas.referenceResolution.x;
//					float scaleY = screenHeight / canvas.referenceResolution.y;
//					float scaleFactor = std::lerp(scaleX, scaleY, canvas.matchWidthOrHeight);
//
//					for (entt::entity child : hc.children)
//					{
//						if (registry.any_of<ui::RectTransform>(child))
//						{
//							CalculateuiLayout(registry, child, rect, scaleFactor, canvasDirty);
//						}
//					}
//				}
//				else
//				{
//					//TODO
//				}
//			}
//		}
//
//
//		const texture2Dasset& texture = assetManager::Getasset(texture2D{ s_uIConfig.texture });
//		float texWidth = static_cast<float>(texture.GetWidth());
//		float texHeight = static_cast<float>(texture.GetHeight());
//
//		auto canvasView = registry.view<ui::Canvas, HierarchyComponent>(entt::exclude<DisabledTag>);
//		for (auto [entity, canvas, hc] : canvasView.each())
//		{
//			if (canvas.isScreenSpace)
//			{
//				for (entt::entity child : hc.children)
//				{
//					CollectuiDrawItems(registry, child, drawList, texWidth, texHeight);
//				}
//			}
//		}
//
//		return drawList;
//	}
//
//	static void CreateuiUniformBuffer()
//	{
//		wgpu::BufferDescriptor bufferDesc;
//		bufferDesc.label = "uiUniformBuffer";
//		bufferDesc.size = sizeof(uiUniforms);
//		bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
//		uiUniformBuffer = graphicsContext::GetDevice().CreateBuffer(&bufferDesc);
//	}
//
//	static void CreateuiStorageBuffer()
//	{
//		constexpr uint64_t maxuiElements = 16384;
//
//		wgpu::BufferDescriptor bufferDesc;
//		bufferDesc.label = "uiStorageBuffer";
//		bufferDesc.size = sizeof(uiDrawItem) * maxuiElements;
//		bufferDesc.usage = wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst;
//		uiStorageBuffer = graphicsContext::GetDevice().CreateBuffer(&bufferDesc);
//	}
//
//	void CreateuiBindGroupLayout()
//	{
//		std::array<wgpu::BindGroupLayoutEntry, 6> entries;
//
//		entries[0].binding = 0;
//		entries[0].visibility = wgpu::ShaderStage::Vertex;
//		entries[0].buffer.type = wgpu::BufferBindingType::Uniform;
//		entries[0].buffer.minBindingSize = sizeof(uiUniforms);
//
//		entries[1].binding = 1;
//		entries[1].visibility = wgpu::ShaderStage::Vertex;
//		entries[1].buffer.type = wgpu::BufferBindingType::ReadOnlyStorage;
//		entries[1].buffer.minBindingSize = sizeof(uiDrawItem);
//
//		entries[2].binding = 2;
//		entries[2].visibility = wgpu::ShaderStage::Fragment;
//		entries[2].texture.sampleType = wgpu::textureSampleType::Float;
//		entries[2].texture.multisampled = false;
//		entries[2].texture.viewDimension = wgpu::textureViewDimension::e2D;
//
//		entries[3].binding = 3;
//		entries[3].visibility = wgpu::ShaderStage::Fragment;
//		entries[3].sampler.type = wgpu::SamplerBindingType::Filtering;
//
//		entries[4].binding = 4;
//		entries[4].visibility = wgpu::ShaderStage::Fragment;
//		entries[4].texture.sampleType = wgpu::textureSampleType::Float;
//		entries[4].texture.multisampled = false;
//		entries[4].texture.viewDimension = wgpu::textureViewDimension::e2D;
//
//		entries[5].binding = 5;
//		entries[5].visibility = wgpu::ShaderStage::Fragment;
//		entries[5].sampler.type = wgpu::SamplerBindingType::Filtering;
//
//		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
//		bindGroupLayoutDesc.label = "uiBindGroupLayout";
//		bindGroupLayoutDesc.entryCount = entries.size();
//		bindGroupLayoutDesc.entries = entries.data();
//
//		s_uIBindGroupLayout = graphicsContext::GetDevice().CreateBindGroupLayout(&bindGroupLayoutDesc);
//	}
//
//	static void CreateuiBindGroup()
//	{
//		std::array<wgpu::BindGroupEntry, 6> entries;
//
//		entries[0].binding = 0;
//		entries[0].buffer = uiUniformBuffer;
//		entries[0].offset = 0;
//		entries[0].size = uiUniformBuffer.GetSize();
//		
//		entries[1].binding = 1;
//		entries[1].buffer = uiStorageBuffer;
//		entries[1].offset = 0;
//		entries[1].size = uiStorageBuffer.GetSize();
//
//		entries[2].binding = 2;
//		entries[2].textureView = assetManager::Getasset(texture2D{ s_uIConfig.texture }).GettextureView();
//
//		entries[3].binding = 3;
//		entries[3].sampler = SamplerPool::GetSampler({ textureFilter::Point, textureWrap::Repeat });
//
//		entries[4].binding = 4;
//		entries[4].textureView = assetManager::Getasset(RESOURCES::TEXTURE::DEFAULT_FONT_ATLAS).GettextureView();
//
//		entries[5].binding = 5;
//		entries[5].sampler = SamplerPool::GetSampler({ textureFilter::Bilinear, textureWrap::Clamp });
//
//		wgpu::BindGroupDescriptor bindGroupDesc;
//		bindGroupDesc.label = "uiBindGroup";
//		bindGroupDesc.layout = s_uIBindGroupLayout;
//		bindGroupDesc.entryCount = entries.size();
//		bindGroupDesc.entries = entries.data();
//		uiBindGroup = graphicsContext::GetDevice().CreateBindGroup(&bindGroupDesc);
//	}
//
//	void CreateuiPipeline()
//	{
//		const Shaderasset& shader = assetManager::Getasset<Shaderasset>(RESOURCES::SHADER::ui);
//
//		wgpu::RenderPipelineDescriptor pipelineDesc;
//		pipelineDesc.label = "ui Shader Pipeline";
//
//		pipelineDesc.vertex.bufferCount = 0;
//		pipelineDesc.vertex.buffers = nullptr;
//		pipelineDesc.vertex.module = shader.GetShaderModule();
//		pipelineDesc.vertex.entryPoint = "vs_main";
//
//		pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
//		pipelineDesc.primitive.frontFace = wgpu::FrontFace::CW;
//		pipelineDesc.primitive.cullMode = wgpu::CullMode::Back;
//
//		wgpu::BlendState blendState;
//		blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
//		blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
//		blendState.color.operation = wgpu::BlendOperation::Add;
//		blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
//		blendState.alpha.dstFactor = wgpu::BlendFactor::One;
//		blendState.alpha.operation = wgpu::BlendOperation::Add;
//
//		wgpu::ColorTargetState colorTarget;
//		colorTarget.format = graphicsContext::GetSurfaceFormat();
//		colorTarget.blend = &blendState;
//		colorTarget.writeMask = wgpu::ColorWriteMask::All;
//
//		wgpu::FragmentState fragmentState;
//		fragmentState.module = shader.GetShaderModule();
//		fragmentState.entryPoint = "fs_main";
//		fragmentState.constantCount = 0;
//		fragmentState.constants = nullptr;
//		fragmentState.targetCount = 1;
//		fragmentState.targets = &colorTarget;
//		pipelineDesc.fragment = &fragmentState;
//
//		pipelineDesc.depthStencil = nullptr;
//		pipelineDesc.multisample.count = 1;
//		pipelineDesc.multisample.mask = ~0u;
//
//		std::array<wgpu::BindGroupLayout, 1> bindGroupLayouts;
//		bindGroupLayouts[0] = s_uIBindGroupLayout;
//
//		wgpu::PipelineLayoutDescriptor layoutDesc;
//		layoutDesc.label = "ui Shader Pipeline Layout";
//		layoutDesc.bindGroupLayoutCount = bindGroupLayouts.size();
//		layoutDesc.bindGroupLayouts = bindGroupLayouts.data();
//		pipelineDesc.layout = graphicsContext::GetDevice().CreatePipelineLayout(&layoutDesc);
//
//		uiPipeline = graphicsContext::GetDevice().CreateRenderPipeline(&pipelineDesc);
//	}
//
//	uipass::uipass()
//	{
//		LoaduitextureAtlas();
//		CreateuiBindGroupLayout();
//		CreateuiPipeline();
//		CreateuiUniformBuffer();
//		CreateuiStorageBuffer();
//		CreateuiBindGroup();
//	}
//
//	void uipass::Execute(wgpu::commandEncoder& encoder, const FrameContext& context)
//	{
//		wgpu::RenderpassColorAttachment color;
//		color.view = context.colorTarget;
//		color.loadOp = wgpu::LoadOp::Load;
//		color.storeOp = wgpu::StoreOp::Store;
//		color.clearValue = wgpu::Color{ 0.0f, 0.0f, 0.0f, 0.0f };
//
//		wgpu::RenderpassDescriptor passDescriptor;
//		passDescriptor.label = "uiRenderpass";
//		passDescriptor.colorAttachmentCount = 1;
//		passDescriptor.colorAttachments = &color;
//		passDescriptor.depthStencilAttachment = nullptr;
//
//		wgpu::RenderpassEncoder pass = encoder.BeginRenderpass(&passDescriptor);
//
//		pass.SetPipeline(uiPipeline);
//		pass.SetBindGroup(0, uiBindGroup, 0, nullptr);
//
//		const uiUniforms orthoUniform = { glm::ortho(0.0f, static_cast<float>(context.width), static_cast<float>(context.height), 0.0f,	-1.0f, 1.0f) };
//		graphicsContext::GetQueue().WriteBuffer(uiUniformBuffer, 0, &orthoUniform, sizeof(uiUniforms));
//
//
//		const std::vector<uiDrawItem> drawList = GenerateuiDrawList(engine::SceneManager::GetActiveScene(), static_cast<float>(context.width), static_cast<float>(context.height));
//		graphicsContext::GetQueue().WriteBuffer(uiStorageBuffer, 0, drawList.data(), drawList.size() * sizeof(uiDrawItem));
//
//		pass.Draw(6, static_cast<uint32_t>(drawList.size()), 0, 0);
//		pass.End();
//	}
//}