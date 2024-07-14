#pragma once

#include "Engine/Renderer/Shader.h"
#include <glm/glm.hpp>

namespace Engine
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;
		
		void SetUniformInt(const std::string& name, int value);
		void SetUniformFloat(const std::string& name, float value);
		void SetUniformFloat2(const std::string& name, const glm::vec2& values);
		void SetUniformFloat3(const std::string& name, const glm::vec3& values);
		void SetUniformFloat4(const std::string& name, const glm::vec4& values);

		void SetUniformMat3f(const std::string& name, const glm::mat3& matrix);
		void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

	private:
		uint32_t m_RendererID = 0;
	};
}