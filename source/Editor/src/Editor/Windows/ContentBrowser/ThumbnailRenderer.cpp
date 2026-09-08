#include "ThumbnailRenderer.h"
#include "Engine/Asset/Resources.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetType.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/Components.h"
#include "Engine/Core/Entity.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/System/TransformSystem.h"
#include "Engine/Graphics/Sprite.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/System/CameraSystem.h"

namespace editor
{
	namespace
	{
		const std::unordered_map<engine::AssetType, glm::ivec2> AssetIconMap
		{
			{engine::AssetType::Material,       {2, 0}},
			{engine::AssetType::Shader,         {2, 0}},
			{engine::AssetType::Audio,          {4, 0}},
			{engine::AssetType::Scene,          {5, 0}},
			{engine::AssetType::Mesh,           {7, 0}},
			{engine::AssetType::Unknown,        {6, 0}},
		};

		const std::unordered_map<EditorIcon, glm::ivec2> EditorIconMap
		{
				{EditorIcon::EmptyDirectory, {0, 0}},
				{EditorIcon::Directory,      {1, 0}},
				{EditorIcon::Unknown,        {6, 0}},
		};

		constexpr glm::ivec2 m_SpritesheetSize{ 1024, 1024 };
		constexpr glm::ivec2 m_CellSize{ 64, 64 };

		std::unordered_map<engine::AssetType, engine::Sprite> m_IconSprites;
		std::unordered_map<EditorIcon, engine::Sprite> m_EditorSprites;

		std::unordered_map<engine::Uuid, Thumbnail> m_ThumbnailCache;

		engine::SceneAsset scene;
		engine::Entity previewEntity;
		engine::Renderer renderer;


		engine::CameraComponent camera;
		engine::TransformComponent cameraTransform;

		struct PreviewRequest 
		{
			engine::Mesh mesh;
			engine::Material material;
		};

		constexpr int AtlasSizePx = 2048;
		constexpr int CellsPerSide = AtlasSizePx / 64;

		wgpu::Texture m_AtlasTexture;
		wgpu::TextureView m_AtlasView;
		int m_NextFreeSlot = 0;

		wgpu::Texture m_ScratchpadTexture;
		wgpu::TextureView m_ScratchpadView;
		wgpu::Texture m_DepthTexture;
		wgpu::TextureView m_DepthView;
	}

	static const engine::Sprite& GetIcon(engine::AssetType assetType) 
	{
		if (m_IconSprites.contains(assetType))
		{
			return m_IconSprites.at(assetType);
		}
		return m_IconSprites.at(engine::AssetType::Unknown);
	}

	void ThumbnailRenderer::Initialize()
	{
		{
			camera.backgroundMode = engine::CameraComponent::Background::Color;
			camera.projectionType = engine::CameraComponent::Projection::Perspective;
			camera.clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

			cameraTransform.position = { 0.0f, 0.0f, 3.0f };
			cameraTransform.rotation = { 0.0f, 0.0f, 0.0f };
		}
		
		{
			previewEntity = scene.CreateEntity<engine::TransformComponent, engine::WorldTransformComponent, engine::MeshComponent, engine::TransformDirty>("PreviewEntity");
			auto& mc = previewEntity.GetComponent<engine::MeshComponent>();
			mc.mesh = engine::Mesh{ RESOURCES::MESH::SPHERE };
			mc.materials = { engine::Material{ RESOURCES::MATERIAL::PINK } };

			auto& tc = previewEntity.GetComponent<engine::TransformComponent>();
			tc.rotation = glm::radians(glm::vec3{ 15.0f, 45.0f, 0.0f });
		}

		std::vector<engine::RenderPass> passes;
		passes.emplace_back(engine::pass::Background());
		passes.emplace_back(engine::pass::Opaque());

		renderer.Initialize(64, 64, passes);

		for (const auto& [type, coords] : AssetIconMap)
		{
			m_IconSprites.emplace(type, engine::Sprite::CreateFromGrid(RESOURCES::TEXTURE::EDITOR_ICONS, m_SpritesheetSize, coords, m_CellSize));
		}

		for (const auto& [iconType, coords] : EditorIconMap)
		{
			m_EditorSprites.emplace(iconType, engine::Sprite::CreateFromGrid(RESOURCES::TEXTURE::EDITOR_ICONS, m_SpritesheetSize, coords, m_CellSize));
		}

		wgpu::TextureDescriptor atlasDesc;
		atlasDesc.label = "ThumbnailAtlas";
		atlasDesc.dimension = wgpu::TextureDimension::e2D;
		atlasDesc.sampleCount = 1;
		atlasDesc.mipLevelCount = 1;
		atlasDesc.size = { AtlasSizePx, AtlasSizePx, 1 };
		atlasDesc.format = engine::GraphicsContext::GetSurfaceFormat();
		atlasDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
		m_AtlasTexture = engine::GraphicsContext::GetDevice().CreateTexture(&atlasDesc);
		m_AtlasView = m_AtlasTexture.CreateView();

		wgpu::TextureDescriptor scratchDesc;
		scratchDesc.label = "ThumbnailScratchpad";
		scratchDesc.dimension = wgpu::TextureDimension::e2D;
		scratchDesc.sampleCount = 1;
		scratchDesc.mipLevelCount = 1;
		scratchDesc.size = { 64, 64, 1 };
		scratchDesc.format = engine::GraphicsContext::GetSurfaceFormat();
		scratchDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc;
		m_ScratchpadTexture = engine::GraphicsContext::GetDevice().CreateTexture(&scratchDesc);
		m_ScratchpadView = m_ScratchpadTexture.CreateView();

		wgpu::TextureDescriptor depthDesc;
		depthDesc.label = "ThumbnailDepth";
		depthDesc.dimension = wgpu::TextureDimension::e2D;
		depthDesc.size = { 64, 64, 1 };
		depthDesc.format = wgpu::TextureFormat::Depth24Plus;
		depthDesc.mipLevelCount = 1;
		depthDesc.sampleCount = 1;
		depthDesc.usage = wgpu::TextureUsage::RenderAttachment;
		m_DepthTexture = engine::GraphicsContext::GetDevice().CreateTexture(&depthDesc);
		m_DepthView = m_DepthTexture.CreateView();
	}

	static Thumbnail RenderToAtlas(const PreviewRequest& request)
	{
		int slot = m_NextFreeSlot++;
		int x = (slot % CellsPerSide) * 64;
		int y = (slot / CellsPerSide) * 64;

		auto& mc = previewEntity.GetComponent<engine::MeshComponent>();

		mc.mesh = request.mesh;
		mc.materials[0] = request.material;

		const engine::MeshAsset& mesh = engine::AssetManager::GetAsset<engine::MeshAsset>(mc.mesh);
		float distance = glm::length(mesh.aabb.max - mesh.aabb.min);

		cameraTransform.position = { 0.0f, 0.0f, -distance };

		engine::CameraSystem::Update(scene.GetRegistry(), 1.0f);
		engine::TransformSystem::Update(scene);

		renderer.RenderScene(scene, camera, m_ScratchpadTexture.CreateView());

		// Copy to Atlas
		wgpu::TexelCopyTextureInfo src;
		src.texture = m_ScratchpadTexture;
		src.mipLevel = 0;
		src.origin = { 0, 0, 0 };

		wgpu::TexelCopyTextureInfo dst;
		dst.texture = m_AtlasTexture;
		dst.mipLevel = 0;
		dst.origin = { (uint32_t)x, (uint32_t)y, 0 };

		wgpu::Extent3D copySize = { 64, 64, 1 };

		// TODO: batch commands and send to gpu once, dont create encoder per thumbnail
		auto encoder = engine::GraphicsContext::GetDevice().CreateCommandEncoder({});
		encoder.CopyTextureToTexture(&src, &dst, &copySize);
		auto cmd = encoder.Finish();
		engine::GraphicsContext::GetQueue().Submit(1, &cmd);

		Thumbnail thumb;
		thumb.view = m_AtlasView;
		thumb.uv_min = { static_cast<float>(x) / AtlasSizePx, static_cast<float>(y) / AtlasSizePx };
		thumb.uv_max = { static_cast<float>(x + 64) / AtlasSizePx, static_cast<float>(y + 64) / AtlasSizePx };
		return thumb;
	}

	//TODO: use template asset handle... dont pass asset type
	const Thumbnail& ThumbnailRenderer::GetThumbnail(engine::Uuid id, engine::AssetType type)
	{
		if (m_ThumbnailCache.contains(id))
		{
			return m_ThumbnailCache[id];
		}

		Thumbnail thumbnail;


		switch (type)
		{
		case engine::AssetType::Texture:
			thumbnail.view = engine::AssetManager::GetAsset(engine::Texture2D{ id }).GetTextureView();
			break;
		case engine::AssetType::Material:
			thumbnail = RenderToAtlas({ engine::Mesh{RESOURCES::MESH::SPHERE}, engine::Material{id} });
			break;
		case engine::AssetType::Mesh:
			thumbnail = RenderToAtlas({ engine::Mesh{id}, engine::Material{RESOURCES::MATERIAL::WHITE} });
			break;
		default:
			const engine::Sprite& sprite = GetIcon(type);
			const engine::Texture2DAsset& texture = engine::AssetManager::GetAsset(sprite.GetTexture());
			thumbnail = { texture.GetTextureView(), sprite.GetUVMin(), sprite.GetUVMax() };
			break;
		}

		m_ThumbnailCache[id] = thumbnail;
		return m_ThumbnailCache[id];
	}

	const Thumbnail& ThumbnailRenderer::GetDirectoryThumbnail(bool isEmpty)
	{
		EditorIcon iconType = isEmpty ? EditorIcon::EmptyDirectory : EditorIcon::Directory;
		const engine::Sprite& sprite = m_EditorSprites.at(iconType);
		const engine::Texture2DAsset& texture = engine::AssetManager::GetAsset<engine::Texture2DAsset>(sprite.GetTexture());

		// You could cache this, but constructing the struct is essentially free
		static Thumbnail thumb;
		thumb = { texture.GetTextureView(), sprite.GetUVMin(), sprite.GetUVMax() };
		return thumb;
	}
}