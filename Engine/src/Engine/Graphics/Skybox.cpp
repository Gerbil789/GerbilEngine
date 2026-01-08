#include "enginepch.h"
#include "Skybox.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Graphics/SamplerPool.h"

namespace Engine
{
  Skybox::Skybox()
  {
		m_CubemapTexture = TextureImporter::LoadCubeMapTexture("Resources/Engine/textures/skybox/sky_02_2k");
    CreateBindGroup();
  }

  void Skybox::CreateBindGroup()
  {
		std::array<wgpu::BindGroupEntry, 2> entries;
		
		{
			wgpu::BindGroupEntry entry{};
			entry.binding = 0;
			entry.sampler = SamplerPool::GetSampler(TextureFilter::Bilinear, TextureWrap::Clamp);
			entries[0] = entry;
		}

		{
			wgpu::BindGroupEntry entry{};
			entry.binding = 1;
			entry.textureView = m_CubemapTexture->GetTextureView();
			entries[1] = entry;
		}

		wgpu::BindGroupDescriptor bindGroupDesc{};
		bindGroupDesc.label = { "SkyboxBindGroup", WGPU_STRLEN };
		bindGroupDesc.layout = m_SkyboxShader.GetBindGroupLayout();
		bindGroupDesc.entryCount = 2;
		bindGroupDesc.entries = entries.data();

		m_BindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);
	}
}