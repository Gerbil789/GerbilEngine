#include "enginepch.h"
#include "OpenGLShader.h"

#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <fstream>

namespace Engine
{
	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;

		ENGINE_ASSERT(false, "Unknown shader type!");
		return 0;
	}


	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);

		// Extract name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);
	}


	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) : m_Name(name)
	{
		std::unordered_map<GLenum, std::string> shaderSources;
		shaderSources[GL_VERTEX_SHADER] = vertexSrc;
		shaderSources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(shaderSources);
	}


	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); // Start of shader type declaration line

		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			ENGINE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1; // Start of shader type name (after "#type " keyword)
			std::string type = source.substr(begin, eol - begin);
			ENGINE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			ENGINE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos); // Start of next shader type declaration line

			shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();
		ENGINE_ASSERT(shaderSources.size() <= 2, "Only 2 shaders are currently supported");
		std::array<GLenum, 2> glShaderIDs; // [0] = vertex, [1] = fragment
		int glShaderIDIndex = 0;

		for (auto& kv : shaderSources) {
			GLenum type = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(type);

			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				ENGINE_LOG_ERROR("{0}", infoLog.data());
				ENGINE_ASSERT(false, "Shader compilation failure!");
				break;
			}

			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);

			for (auto id : glShaderIDs)
				glDeleteShader(id);

			ENGINE_LOG_ERROR("{0}", infoLog.data());
			ENGINE_ASSERT(false, "Shader link failure!");
			return;
		}

		for (auto id : glShaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}

		m_RendererID = program;
	}


	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}


	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		std::string result;

		std::ifstream in(filepath, std::ios::in | std::ios::binary);

		if (!in)
		{
			ENGINE_LOG_ERROR("Could not open file '{0}'", filepath);
			return "";
		}

		in.seekg(0, std::ios::end);
		size_t size = in.tellg();
		result.resize(size);
		in.seekg(0, std::ios::beg);
		in.read(&result[0], size);

		in.close();

		return result;
	}


	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}



	void OpenGLShader::SetUniformInt(const std::string& name, int value)
	{
		glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
	}

	void OpenGLShader::SetUniformFloat(const std::string& name, float value)
	{
		glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), value);
	}

	void OpenGLShader::SetUniformFloat2(const std::string& name, const glm::vec2& values)
	{
		glUniform2f(glGetUniformLocation(m_RendererID, name.c_str()), values.x, values.y);
	}

	void OpenGLShader::SetUniformFloat3(const std::string& name, const glm::vec3& values)
	{
		glUniform3f(glGetUniformLocation(m_RendererID, name.c_str()), values.x, values.y, values.z);
	}

	void OpenGLShader::SetUniformFloat4(const std::string& name, const glm::vec4& values)
	{
		glUniform4f(glGetUniformLocation(m_RendererID, name.c_str()), values.x, values.y, values.z, values.w);
	}

	void OpenGLShader::SetUniformMat3f(const std::string& name, const glm::mat3& matrix)
	{
		glUniformMatrix3fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	}
	void OpenGLShader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	}

}