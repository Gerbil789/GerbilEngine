#include "ThumbnailRenderer.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/Components.h"
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
		const std::unordered_map<Icon, glm::ivec2> IconMap
		{
			{ Icon::Unknown,{ 6, 0 } },
			{ Icon::Folder,{ 1, 0 } },
			{ Icon::EmptyFolder,{ 0, 0 } },
			{ Icon::Texture,{ 2, 0 } },
			{ Icon::Text,{ 2, 0 } },
			{ Icon::Material,{ 2, 0 } },
			{ Icon::Mesh,{ 7, 0 } },
			{ Icon::Scene,{ 5, 0 } },
			{ Icon::Audio,{ 4, 0 } },
		};

		constexpr glm::ivec2 m_SpritesheetSize{ 1024, 1024 };
		constexpr glm::ivec2 m_CellSize{ 64, 64 };

		std::unordered_map<Icon, engine::Sprite> m_Icons;
		std::unordered_map<engine::Uuid, engine::Sprite> m_ThumbnailCache;

		engine::Renderer renderer;
		engine::CameraComponent camera;

		constexpr int AtlasSizePx = 2048;
		constexpr int CellsPerSide = AtlasSizePx / 64;

		engine::Texture2D m_AtlasTexture = engine::Texture2D::EDITOR_THUMBNAIL_ATLAS;
		int m_NextFreeSlot = 0;

		wgpu::Texture m_ScratchpadTexture;
		wgpu::TextureView m_ScratchpadView;
		wgpu::Texture m_DepthTexture;
		wgpu::TextureView m_DepthView;
	}

	void ThumbnailRenderer::Initialize()
	{
		camera.backgroundMode = engine::CameraComponent::Background::Color;
		camera.projectionType = engine::CameraComponent::Projection::Perspective;
		camera.clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
		engine::CameraSystem::UpdateCameraProjectionMatrix(camera, 1.0f);

		renderer.Initialize(64, 64);
		renderer.AddPass<engine::BackgroundPass>();
		renderer.AddPass<engine::OpaquePass>();

		for (const auto& [icon, coords] : IconMap)
		{
			m_Icons.emplace(icon, engine::Sprite::CreateFromGrid(engine::Texture2D::EDITOR_ICONS, m_SpritesheetSize, coords, m_CellSize));
		}

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

	static engine::Sprite RenderToAtlas(engine::Mesh mesh, engine::MaterialInstance materialInstance)
	{
		const engine::MeshAsset& meshData = mesh.Get();

		const engine::AABB& aabb = meshData.aabb;
		float distance = glm::length(aabb.max - aabb.min) + 1.0f;

		engine::TransformComponent cameraTransform;
		cameraTransform.position = { 0.0f, 0.0f, distance };
		engine::CameraSystem::UpdateCameraViewMatrix(camera, cameraTransform);


		engine::TransformComponent entityTransform;
		entityTransform.rotation = { 0.0f, glm::radians(20.0f), 0.0f };
		const glm::mat4 worldMatrix = engine::TransformSystem::CalculateLocalPositionMatrix(entityTransform);

		engine::DrawList list;

		const auto& subMeshes = meshData.subMeshes;

    for (uint32_t i = 0; i < subMeshes.size(); ++i)
    {
      const auto& subMesh = subMeshes[i];

      list.items.emplace_back(mesh, materialInstance, i, subMesh.indexCount, subMesh.firstIndex);
      list.transforms.push_back(worldMatrix);
    }

		renderer.Render(list, &camera, m_ScratchpadTexture.CreateView());

		// Copy to Atlas
		wgpu::TexelCopyTextureInfo src;
		src.texture = m_ScratchpadTexture;
		src.mipLevel = 0;
		src.origin = { 0, 0, 0 };

		int slot = m_NextFreeSlot++;
		int x = (slot % CellsPerSide) * 64;
		int y = (slot / CellsPerSide) * 64;

		wgpu::TexelCopyTextureInfo dst;
		dst.texture = m_AtlasTexture.Get().texture;
		dst.mipLevel = 0;
		dst.origin = { (uint32_t)x, (uint32_t)y, 0 };

		wgpu::Extent3D copySize = { 64, 64, 1 };

		auto encoder = engine::GraphicsContext::GetDevice().CreateCommandEncoder({});
		encoder.CopyTextureToTexture(&src, &dst, &copySize);
		auto cmd = encoder.Finish();
		engine::GraphicsContext::GetQueue().Submit(1, &cmd);

		engine::Sprite sprite;
		sprite.texture = m_AtlasTexture;
		sprite.uvMin = { static_cast<float>(x) / AtlasSizePx, static_cast<float>(y) / AtlasSizePx };
		sprite.uvMax = { static_cast<float>(x + 64) / AtlasSizePx, static_cast<float>(y + 64) / AtlasSizePx };
		return sprite;
	}

	const engine::Sprite& ThumbnailRenderer::GetAssetThumbnail(engine::Uuid id, engine::AssetType type)
	{
		if (m_ThumbnailCache.contains(id))
		{
			return m_ThumbnailCache[id];
		}

		engine::Sprite sprite;

		switch (type)
		{
		case engine::AssetType::Texture:
			sprite.texture = engine::Texture2D{ id };
			break;
		case engine::AssetType::Shader:
			sprite = GetEditorIcon(Icon::Text);
			break;
		case engine::AssetType::Material:
			sprite = GetEditorIcon(Icon::Unknown); //TODO
			break;
		case engine::AssetType::MaterialInstance:
			sprite = RenderToAtlas(engine::Mesh::Sphere, engine::MaterialInstance{ id });
			break;
		case engine::AssetType::Mesh:
			sprite = RenderToAtlas(engine::Mesh{ id }, engine::MaterialInstance::White);
			break;
		case engine::AssetType::Scene:
			sprite = GetEditorIcon(Icon::Scene);
			break;
		case engine::AssetType::AudioClip:
			sprite = GetEditorIcon(Icon::Audio);
			break;
		default:
			sprite = GetEditorIcon(Icon::Unknown);
			break;
		}

		m_ThumbnailCache[id] = sprite;
		return m_ThumbnailCache[id];
	}


	const engine::Sprite& ThumbnailRenderer::GetEditorIcon(Icon icon)
	{
		if (m_Icons.contains(icon))
		{
			return m_Icons.at(icon);
		}
		return m_Icons.at(Icon::Unknown);
	}
}