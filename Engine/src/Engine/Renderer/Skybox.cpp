#include "enginepch.h"
#include "Skybox.h"
#include "Engine/Renderer/GraphicsContext.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Asset/Importer/ShaderImporter.h"
#include "Engine/Asset/Importer/TextureImporter.h"

namespace Engine
{
  Skybox::Skybox()
  {
		m_CubemapTexture = TextureImporter::LoadCubeMapTexture("Resources/Engine/textures/skybox/sky_02_2k");
    CreateBindGroup();
  }

  void Skybox::CreateBindGroup()
  {
		{
			// create sampler
			wgpu::SamplerDescriptor samplerDesc = {};
			samplerDesc.label = { "SkyboxSampler", WGPU_STRLEN };
			samplerDesc.addressModeU = wgpu::AddressMode::Repeat;
			samplerDesc.addressModeV = wgpu::AddressMode::Repeat;
			samplerDesc.addressModeW = wgpu::AddressMode::ClampToEdge;
			samplerDesc.magFilter = wgpu::FilterMode::Linear;
			samplerDesc.minFilter = wgpu::FilterMode::Linear;
			samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
			samplerDesc.lodMinClamp = 0.0f;
			samplerDesc.lodMaxClamp = 1.0f;
			samplerDesc.compare = wgpu::CompareFunction::Undefined;
			samplerDesc.maxAnisotropy = 1;
			m_Sampler = GraphicsContext::GetDevice().createSampler(samplerDesc);
		}

		std::array<wgpu::BindGroupEntry, 2> entries;
		
		{
			wgpu::BindGroupEntry entry{};
			entry.binding = 0;
			entry.sampler = m_Sampler;
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