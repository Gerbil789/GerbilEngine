#pragma once

#include <string>

namespace Engine 
{
	class Shader 
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual std::string GetName() const = 0;

		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		static Ref<Shader> Create(const std::string& filepath);
	};

	class ShaderLibrary
	{
	public:
		void Add(const std::string& name, Ref<Shader> shader);
		void Add(Ref<Shader> shader);
		Ref<Shader> Load(const std::string& filepath);
		Ref<Shader> Load(const std::string& name, const std::string& filepath);

		Ref<Shader> Get(const std::string& name);

		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}