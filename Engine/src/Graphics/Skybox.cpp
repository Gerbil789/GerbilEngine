#include "enginepch.h"
#include "Engine/Graphics/Skybox.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Graphics/SamplerPool.h"

namespace Engine
{
  Skybox::Skybox()
  {
		m_CubemapTexture = TextureImporter::LoadCubeMapTexture("Resources/Engine/textures/skybox/sky_02_2k");
		m_HDRTexture = TextureImporter::LoadTexture2D("Resources/Engine/hdr/PG2/lebombo_prefiltered_env_map_001.hdr");
    CreateBindGroup();
  }

  void Skybox::CreateBindGroup()
  {
		std::array<wgpu::BindGroupEntry, 2> entries;
		
		{
			//wgpu::SamplerDescriptor desc{};
			//desc.minFilter = minMag;
			//desc.magFilter = minMag;
			//desc.mipmapFilter = mip;
			//desc.addressModeU = mode;
			//desc.addressModeV = mode;
			//desc.addressModeW = mode;
			//desc.maxAnisotropy = 1;
			//return device.createSampler(desc);


			wgpu::BindGroupEntry entry{};
			entry.binding = 0;
			entry.sampler = SamplerPool::GetSampler(TextureFilter::Point, TextureWrap::Clamp);
			entries[0] = entry;
		}

		//{
		//	wgpu::BindGroupEntry entry{};
		//	entry.binding = 1;
		//	entry.textureView = m_CubemapTexture->GetTextureView();
		//	entries[1] = entry;
		//}

		{
			wgpu::BindGroupEntry entry{};
			entry.binding = 1;
			entry.textureView = m_HDRTexture->GetTextureView();
			entries[1] = entry;
		}

		wgpu::BindGroupDescriptor bindGroupDesc{};
		bindGroupDesc.label = { "SkyboxBindGroup", WGPU_STRLEN };
		bindGroupDesc.layout = m_EnvironmentShader.GetBindGroupLayout();
		bindGroupDesc.entryCount = 2;
		bindGroupDesc.entries = entries.data();

		m_BindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);
	}
}