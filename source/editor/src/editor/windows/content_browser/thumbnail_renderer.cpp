#include "thumbnail_renderer.h"
#include "engine/asset/asset_manager.h"
#include "engine/core/components.h"
#include "engine/graphics/graphics_context.h"
#include "engine/graphics/renderer/renderer.h"
#include "engine/graphics/texture/texture_2d.h"
#include "engine/system/transform_system.h"
#include "engine/graphics/sprite.h"
#include "engine/graphics/mesh.h"
#include "engine/system/camera_system.h"

namespace editor
{
	namespace
	{
		const std::unordered_map<Icon, glm::ivec2> IconMap
		{
			{ Icon::Unknown,{ 6, 0 } },
			{ Icon::Folder,{ 1, 0 } },
			{ Icon::EmptyFolder,{ 0, 0 } },
			{ Icon::texture,{ 2, 0 } },
			{ Icon::Text,{ 2, 0 } },
			{ Icon::Material,{ 2, 0 } },
			{ Icon::Mesh,{ 7, 0 } },
			{ Icon::Scene,{ 5, 0 } },
			{ Icon::audio,{ 4, 0 } },
		};

		constexpr glm::ivec2 m_spritesheetSize{ 1024, 1024 };
		constexpr glm::ivec2 m_cellSize{ 64, 64 };

		std::unordered_map<Icon, engine::Sprite> m_icons;
		std::unordered_map<engine::Uuid, engine::Sprite> m_thumbnailCache;

		engine::Renderer<engine::BackgroundPass, engine::OpaquePass> renderer;
		engine::component::Camera camera;

		constexpr int AtlasSizePx = 2048;
		constexpr int CellsPerSide = AtlasSizePx / 64;

		engine::TextureHandle m_atlastexture = engine::texture::editorThumbnailAtlas;
		int m_nextFreeSlot = 0;

		wgpu::Texture m_scratchpadtexture;
		wgpu::TextureView m_scratchpadView;
		wgpu::Texture m_depthtexture;
		wgpu::TextureView m_depthView;
	}

	void ThumbnailRenderer::Initialize()
	{
		camera.backgroundMode = engine::component::Camera::Background::Color;
		camera.projection = engine::component::Camera::Projection::Perspective;
		camera.clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
		engine::Camerasystem::UpdateCameraProjectionMatrix(camera, 1.0f);

		renderer.Initialize(64, 64);

		for (const auto& [icon, coords] : IconMap)
		{
			m_icons.emplace(icon, engine::Sprite::CreateFromGrid(engine::texture::editorIcons, m_spritesheetSize, coords, m_cellSize));
		}

		wgpu::TextureDescriptor scratchDesc;
		scratchDesc.label = "ThumbnailScratchpad";
		scratchDesc.dimension = wgpu::TextureDimension::e2D;
		scratchDesc.sampleCount = 1;
		scratchDesc.mipLevelCount = 1;
		scratchDesc.size = { 64, 64, 1 };
		scratchDesc.format = engine::graphicsContext::GetSurfaceFormat();
		scratchDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc;
		m_scratchpadtexture = engine::graphicsContext::GetDevice().CreateTexture(&scratchDesc);
		m_scratchpadView = m_scratchpadtexture.CreateView();

		wgpu::TextureDescriptor depthDesc;
		depthDesc.label = "ThumbnailDepth";
		depthDesc.dimension = wgpu::TextureDimension::e2D;
		depthDesc.size = { 64, 64, 1 };
		depthDesc.format = wgpu::TextureFormat::Depth24Plus;
		depthDesc.mipLevelCount = 1;
		depthDesc.sampleCount = 1;
		depthDesc.usage = wgpu::TextureUsage::RenderAttachment;
		m_depthtexture = engine::graphicsContext::GetDevice().CreateTexture(&depthDesc);
		m_depthView = m_depthtexture.CreateView();
	}

	static engine::Sprite RenderToAtlas(engine::MeshHandle mesh, engine::MaterialInstanceHandle materialInstance)
	{
		const engine::Mesh& meshData = mesh.Get();

		const engine::AABB& aabb = meshData.aabb;
		float distance = glm::length(aabb.max - aabb.min) + 1.0f;

		engine::component::Transform cameraTransform;
		cameraTransform.position = { 0.0f, 0.0f, distance };
		engine::Camerasystem::UpdateCameraViewMatrix(camera, cameraTransform);

		engine::component::Transform entityTransform;
		entityTransform.rotation = { 0.0f, glm::radians(20.0f), 0.0f };
		const glm::mat4 worldMatrix = engine::Transformsystem::CalculateLocalMatrix(entityTransform);

		engine::DrawList list;

		const auto& submeshes = meshData.submeshes;

    for (uint32_t i = 0; i < submeshes.size(); ++i)
    {
      const auto& subMesh = submeshes[i];

      list.items.emplace_back(mesh, materialInstance, i, subMesh.indexCount, subMesh.firstIndex);
      list.transforms.push_back(worldMatrix);
    }

		renderer.Render(list, &camera, m_scratchpadtexture.CreateView());

		// Copy to Atlas
		wgpu::TexelCopyTextureInfo src;
		src.texture = m_scratchpadtexture;
		src.mipLevel = 0;
		src.origin = { 0, 0, 0 };

		int slot = m_nextFreeSlot++;
		int x = (slot % CellsPerSide) * 64;
		int y = (slot / CellsPerSide) * 64;

		wgpu::TexelCopyTextureInfo dst;
		dst.texture = m_atlastexture.Get().texture;
		dst.mipLevel = 0;
		dst.origin = { (uint32_t)x, (uint32_t)y, 0 };

		wgpu::Extent3D copySize = { 64, 64, 1 };

		auto encoder = engine::graphicsContext::GetDevice().CreateCommandEncoder({});
		encoder.CopyTextureToTexture(&src, &dst, &copySize);
		auto cmd = encoder.Finish();
		engine::graphicsContext::GetQueue().Submit(1, &cmd);

		engine::Sprite sprite;
		sprite.textureHandle = m_atlastexture;
		sprite.uvMin = { static_cast<float>(x) / AtlasSizePx, static_cast<float>(y) / AtlasSizePx };
		sprite.uvMax = { static_cast<float>(x + 64) / AtlasSizePx, static_cast<float>(y + 64) / AtlasSizePx };
		return sprite;
	}

	const engine::Sprite& ThumbnailRenderer::GetassetThumbnail(engine::Uuid id, engine::AssetType type)
	{
		if (m_thumbnailCache.contains(id))
		{
			return m_thumbnailCache[id];
		}

		engine::Sprite sprite;

		switch (type)
		{
		case engine::AssetType::texture:
			sprite.textureHandle = engine::TextureHandle{ id };
			break;
		case engine::AssetType::Shader:
			sprite = GeteditorIcon(Icon::Text);
			break;
		case engine::AssetType::Material:
			sprite = GeteditorIcon(Icon::Unknown); //TODO
			break;
		case engine::AssetType::MaterialInstance:
			sprite = RenderToAtlas(engine::mesh::Sphere, engine::MaterialInstanceHandle{ id });
			break;
		case engine::AssetType::Mesh:
			sprite = RenderToAtlas(engine::MeshHandle{ id }, engine::material_instance::White);
			break;
		case engine::AssetType::Scene:
			sprite = GeteditorIcon(Icon::Scene);
			break;
		case engine::AssetType::AudioClip:
			sprite = GeteditorIcon(Icon::audio);
			break;
		default:
			sprite = GeteditorIcon(Icon::Unknown);
			break;
		}

		m_thumbnailCache[id] = sprite;
		return m_thumbnailCache[id];
	}


	const engine::Sprite& ThumbnailRenderer::GeteditorIcon(Icon icon)
	{
		if (m_icons.contains(icon))
		{
			return m_icons.at(icon);
		}
		return m_icons.at(Icon::Unknown);
	}
}