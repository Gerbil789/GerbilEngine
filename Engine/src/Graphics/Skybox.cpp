#include "enginepch.h"
#include "Engine/Graphics/Skybox.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Graphics/SamplerPool.h"
#include "Engine/Graphics/Texture.h"

namespace Engine
{
  Skybox::Skybox()
  {
    CreateBindGroup();
  }

  void Skybox::CreateBindGroup()
  {
		std::array<wgpu::BindGroupEntry, 2> entries;
		
		{
			wgpu::BindGroupEntry entry;
			entry.binding = 0;
			entry.sampler = SamplerPool::GetSampler(TextureFilter::Point, TextureWrap::Clamp);
			entries[0] = entry;
		}

		{
			CubeMapTexture* cubemap = Engine::TextureImporter::LoadCubeMapTexture("Resources/Engine/hdr/PG2/lebombo_8k.hdr");

			wgpu::BindGroupEntry entry;
			entry.binding = 1;
			entry.textureView = cubemap->GetTextureView();
			entries[1] = entry;

			delete cubemap;
		}

		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.label = { "SkyboxBindGroup", WGPU_STRLEN };
		bindGroupDesc.layout = m_SkyboxShader.GetBindGroupLayout();
		bindGroupDesc.entryCount = entries.size();
		bindGroupDesc.entries = entries.data();

		m_BindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);
	}
}