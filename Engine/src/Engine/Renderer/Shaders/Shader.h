#pragma once

#include <filesystem>
#include <webgpu/webgpu.hpp>

namespace GroupID 
{
	constexpr int Frame = 0;
	constexpr int Model = 1;
	constexpr int Material = 2;
}

namespace Engine 
{
	enum class ShaderParamType
	{
		Float, Vec3, Vec4, Texture2D, Sampler
	};

	struct ShaderParameter
	{
		std::string name;
		uint32_t group;
		uint32_t binding;
		ShaderParamType type;
		size_t size;
		size_t offset;
	};

	class Shader
	{
	public:
		virtual ~Shader() = default;

		wgpu::RenderPipeline GetRenderPipeline() const { return m_RenderPipeline; }
		wgpu::BindGroupLayout GetMaterialBindGroupLayout() const { return m_MaterialBindGroupLayout; }
		const std::vector<ShaderParameter>& GetParameters() const { return m_Parameters; }
		size_t GetUniformBufferSize() const 
		{ 
			return m_CurrentUniformBufferOffset; 
		}
		static wgpu::ShaderModule LoadShader(const std::filesystem::path& path);
	protected:
		void AddParameter(const std::string& name, uint32_t group, uint32_t binding, ShaderParamType type, size_t size);
		wgpu::BindGroupLayout CreateMaterialBindGroupLayout();

		inline size_t AlignTo(size_t offset, size_t alignment)
		{
			return (offset + alignment - 1) & ~(alignment - 1);
		}
		size_t m_CurrentUniformBufferOffset = 0;
	protected:
		std::vector<ShaderParameter> m_Parameters;

		wgpu::RenderPipeline m_RenderPipeline;
		wgpu::BindGroupLayout m_MaterialBindGroupLayout;
	};
}