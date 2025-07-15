#include "enginepch.h"
#include "RenderUtils.h"
#include <glm/glm.hpp>
#include "Engine/Core/Application.h"

namespace Engine::RenderUtils
{
  wgpu::Device s_Device;
  wgpu::BindGroupLayout s_ModelBindGroupLayout; // model matrix
	wgpu::BindGroupLayout s_FrameBindGroupLayout; // view, projection, camera position

  void Initialize(wgpu::Device device)
  {
    s_Device = device;

    // Model BindGroupLayout
    {
      wgpu::BindGroupLayoutEntry entry{};
      entry.binding = 0;
      entry.visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
      entry.buffer.type = wgpu::BufferBindingType::Uniform;
      entry.buffer.hasDynamicOffset = false;
      entry.buffer.minBindingSize = sizeof(ModelUniforms);

      wgpu::BindGroupLayoutDescriptor desc{};
      desc.label = { "ModelBindGroupLayout", WGPU_STRLEN };
      desc.entryCount = 1;
      desc.entries = &entry;

      s_ModelBindGroupLayout = s_Device.createBindGroupLayout(desc);
    }

    // Frame BindGroupLayout
    {
      wgpu::BindGroupLayoutEntry entry{};
      entry.binding = 0;
      entry.visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
      entry.buffer.type = wgpu::BufferBindingType::Uniform;
      entry.buffer.hasDynamicOffset = false;
      entry.buffer.minBindingSize = sizeof(FrameUniforms);

      wgpu::BindGroupLayoutDescriptor desc{};
      desc.label = { "FrameBindGroupLayout", WGPU_STRLEN };
      desc.entryCount = 1;
      desc.entries = &entry;

      s_FrameBindGroupLayout = s_Device.createBindGroupLayout(desc);
    }
  }

  wgpu::Buffer RenderUtils::CreateModelBuffer()
  {
    wgpu::BufferDescriptor desc{};
    desc.label = { "ModelUniformBuffer", WGPU_STRLEN };;
    desc.size = sizeof(ModelUniforms);
    desc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
    return s_Device.createBuffer(desc);
  }

  wgpu::BindGroup RenderUtils::CreateModelBindGroup(wgpu::Buffer modelBuffer)
  {
    wgpu::BindGroupEntry entry{};
    entry.binding = 0;
    entry.buffer = modelBuffer;
    entry.offset = 0;
    entry.size = sizeof(ModelUniforms);

    wgpu::BindGroupDescriptor desc{};
    desc.label = { "ModelBindGroup", WGPU_STRLEN };
    desc.layout = s_ModelBindGroupLayout;
    desc.entryCount = 1;
    desc.entries = &entry;

    return s_Device.createBindGroup(desc);
  }

  wgpu::Buffer CreateFrameBuffer()
  {
    wgpu::BufferDescriptor desc{};
    desc.label = { "FrameUniformBuffer", WGPU_STRLEN };;
    desc.size = sizeof(FrameUniforms);
    desc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
    return s_Device.createBuffer(desc);
  }

  wgpu::BindGroup CreateFrameBindGroup(wgpu::Buffer frameBuffer)
  {
    wgpu::BindGroupEntry entry{};
    entry.binding = 0;
    entry.buffer = frameBuffer;
    entry.offset = 0;
    entry.size = sizeof(FrameUniforms);

    wgpu::BindGroupDescriptor desc{};
    desc.label = { "FrameBindGroup", WGPU_STRLEN };
    desc.layout = s_FrameBindGroupLayout;
    desc.entryCount = 1;
    desc.entries = &entry;

    return s_Device.createBindGroup(desc);
  }
}