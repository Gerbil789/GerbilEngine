#include "ThumbnailRenderer.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Graphics/Sprite.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Core/Resources.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Scene/Components.h"
#include "Engine/Asset/AssetType.h"
#include "Engine/Graphics/Mesh.h"

namespace Editor
{
	namespace
	{
		const std::unordered_map<Engine::AssetType, glm::ivec2> AssetIconMap
		{
			{Engine::AssetType::Material,       {2, 0}},
			{Engine::AssetType::Shader,         {2, 0}},
			{Engine::AssetType::Audio,          {4, 0}},
			{Engine::AssetType::Scene,          {5, 0}},
			{Engine::AssetType::Mesh,           {7, 0}},
			{Engine::AssetType::Unknown,        {6, 0}},
		};

		const std::unordered_map<EditorIcon, glm::ivec2> EditorIconMap
		{
				{EditorIcon::EmptyDirectory, {0, 0}},
				{EditorIcon::Directory,      {1, 0}},
				{EditorIcon::Unknown,        {6, 0}},
		};

		constexpr glm::ivec2 m_SpritesheetSize{ 1024, 1024 };
		constexpr glm::ivec2 m_CellSize{ 64, 64 };

		std::unordered_map<Engine::AssetType, Engine::Sprite> m_IconSprites;
		std::unordered_map<EditorIcon, Engine::Sprite> m_EditorSprites;

		std::unordered_map<Engine::Uuid, Thumbnail> m_ThumbnailCache;

		Engine::Scene scene;
		Engine::Entity entity;
		Engine::Camera camera;
		Engine::Renderer renderer;

		struct PreviewRequest 
		{
			Engine::Uuid meshId;
			Engine::Uuid materialId;
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

	static const Engine::Sprite& GetIcon(Engine::AssetType assetType) 
	{
		if (m_IconSprites.contains(assetType))
		{
			return m_IconSprites.at(assetType);
		}
		return m_IconSprites.at(Engine::AssetType::Unknown);
	}

	void ThumbnailRenderer::Initialize()
	{
		camera.SetBackground(Engine::Camera::Background::Color);
		camera.SetProjection(Engine::Camera::Projection::Perspective);
		camera.SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		camera.SetAspectRatio(1.0f);
		camera.SetPosition({ 0.0f, 0.0f, 3.0f });
		camera.SetRotation({ 0.0f, 180.0f, 0.0f });

		entity = scene.CreateEntity("PreviewEntity");
		auto& mc = entity.AddComponent<Engine::MeshComponent>();
		mc.meshId = RESOURCES::MESH::SPHERE;
		mc.materials.push_back(RESOURCES::MATERIAL::PINK);

		auto& tc = entity.GetComponent<Engine::TransformComponent>();
		tc.rotation = { 15.0f, 45.0f, 0.0f };
		tc.UpdateMatrix();

		renderer.Initialize();
		renderer.SetFlags(Engine::RenderPassType::Background | Engine::RenderPassType::Opaque);

		for (const auto& [type, coords] : AssetIconMap)
		{
			m_IconSprites.emplace(type, Engine::Sprite::CreateFromGrid(RESOURCES::TEXTURE::EDITOR_ICONS, m_SpritesheetSize, coords, m_CellSize));
		}

		for (const auto& [iconType, coords] : EditorIconMap)
		{
			m_EditorSprites.emplace(iconType, Engine::Sprite::CreateFromGrid(RESOURCES::TEXTURE::EDITOR_ICONS, m_SpritesheetSize, coords, m_CellSize));
		}

		wgpu::TextureDescriptor atlasDesc;
		atlasDesc.label = { "ThumbnailAtlas", WGPU_STRLEN };
		atlasDesc.dimension = wgpu::TextureDimension::_2D;
		atlasDesc.sampleCount = 1;
		atlasDesc.mipLevelCount = 1;
		atlasDesc.size = { AtlasSizePx, AtlasSizePx, 1 };
		atlasDesc.format = Engine::GraphicsContext::GetSurfaceFormat();
		atlasDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
		m_AtlasTexture = Engine::GraphicsContext::GetDevice().createTexture(atlasDesc);
		m_AtlasView = m_AtlasTexture.createView();

		wgpu::TextureDescriptor scratchDesc;
		scratchDesc.label = { "ThumbnailScratchpad", WGPU_STRLEN };
		scratchDesc.dimension = wgpu::TextureDimension::_2D;
		scratchDesc.sampleCount = 1;
		scratchDesc.mipLevelCount = 1;
		scratchDesc.size = { 64, 64, 1 };
		scratchDesc.format = Engine::GraphicsContext::GetSurfaceFormat();
		scratchDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc;
		m_ScratchpadTexture = Engine::GraphicsContext::GetDevice().createTexture(scratchDesc);
		m_ScratchpadView = m_ScratchpadTexture.createView();

		wgpu::TextureDescriptor depthDesc;
		depthDesc.label = { "ThumbnailDepth", WGPU_STRLEN };
		depthDesc.dimension = wgpu::TextureDimension::_2D;
		depthDesc.size = { 64, 64, 1 };
		depthDesc.format = wgpu::TextureFormat::Depth24Plus;
		depthDesc.mipLevelCount = 1;
		depthDesc.sampleCount = 1;
		depthDesc.usage = wgpu::TextureUsage::RenderAttachment;
		m_DepthTexture = Engine::GraphicsContext::GetDevice().createTexture(depthDesc);
		m_DepthView = m_DepthTexture.createView();
	}

	static Thumbnail RenderToAtlas(const PreviewRequest& request)
	{
		int slot = m_NextFreeSlot++;
		int x = (slot % CellsPerSide) * 64;
		int y = (slot / CellsPerSide) * 64;

		auto& mc = entity.GetComponent<Engine::MeshComponent>();

		mc.meshId = request.meshId;
		mc.materials[0] = request.materialId;

		const Engine::Mesh& mesh = Engine::AssetManager::GetAsset<Engine::Mesh>(mc.meshId);
		float distance = glm::length(mesh.aabb.max - mesh.aabb.min);

		camera.SetPosition({ 0.0f, 0.0f, distance });

		renderer.SetColorTarget(m_ScratchpadView);
		renderer.SetDepthTarget(m_DepthView);
		renderer.RenderScene(scene, camera);

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
		auto encoder = Engine::GraphicsContext::GetDevice().createCommandEncoder({});
		encoder.copyTextureToTexture(src, dst, copySize);
		auto cmd = encoder.finish();
		Engine::GraphicsContext::GetQueue().submit(1, &cmd);

		Thumbnail thumb;
		thumb.view = m_AtlasView;
		thumb.uv_min = { static_cast<float>(x) / AtlasSizePx, static_cast<float>(y) / AtlasSizePx };
		thumb.uv_max = { static_cast<float>(x + 64) / AtlasSizePx, static_cast<float>(y + 64) / AtlasSizePx };
		return thumb;
	}

	const Thumbnail& ThumbnailRenderer::GetThumbnail(Engine::Uuid id, Engine::AssetType type)
	{
		if (m_ThumbnailCache.contains(id))
		{
			return m_ThumbnailCache[id];
		}

		Thumbnail thumbnail;

		switch (type)
		{
		case Engine::AssetType::Texture:
			thumbnail.view = Engine::AssetManager::GetAsset<Engine::Texture2D>(id).GetTextureView();
			break;
		case Engine::AssetType::Material:
			thumbnail = RenderToAtlas({ RESOURCES::MESH::SPHERE, id });
			break;
		case Engine::AssetType::Mesh:
			thumbnail = RenderToAtlas({ id, RESOURCES::MATERIAL::WHITE });
			break;
		default:
			const Engine::Sprite& sprite = GetIcon(type);
			const Engine::Texture2D& texture = Engine::AssetManager::GetAsset<Engine::Texture2D>(sprite.GetTexture());
			thumbnail = { texture.GetTextureView(), sprite.GetUVMin(), sprite.GetUVMax() };
			break;
		}

		m_ThumbnailCache[id] = thumbnail;
		return m_ThumbnailCache[id];
	}

	const Thumbnail& ThumbnailRenderer::GetDirectoryThumbnail(bool isEmpty)
	{
		EditorIcon iconType = isEmpty ? EditorIcon::EmptyDirectory : EditorIcon::Directory;
		const Engine::Sprite& sprite = m_EditorSprites.at(iconType);
		const Engine::Texture2D& texture = Engine::AssetManager::GetAsset<Engine::Texture2D>(sprite.GetTexture());

		// You could cache this, but constructing the struct is essentially free
		static Thumbnail thumb;
		thumb = { texture.GetTextureView(), sprite.GetUVMin(), sprite.GetUVMax() };
		return thumb;
	}
}