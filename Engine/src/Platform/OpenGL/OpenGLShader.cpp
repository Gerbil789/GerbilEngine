#include "enginepch.h"
#include "OpenGLShader.h"
#include "Engine/Utils/File.h"

#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <fstream>

#include <shaderc/shaderc.hpp>

namespace Engine
{
	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex") return GL_VERTEX_SHADER;
		if (type == "fragment") return GL_FRAGMENT_SHADER;

		ASSERT(false, "Unknown shader type!");
		return 0;
	}


	OpenGLShader::OpenGLShader(const std::filesystem::path& path) : Shader(path)
	{
		ENGINE_PROFILE_FUNCTION();

		auto source = ReadFile(path.string());
		if (!source) { return; }

		Compile(PreProcess(*source));
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(std::string& source)
	{
		ENGINE_PROFILE_FUNCTION();

		IncludeLibs(source); 

		std::unordered_map<GLenum, std::string> shaderSources;
		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); // Start of shader type declaration line

		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1; // Start of shader type name (after "#type " keyword)
			std::string type = source.substr(begin, eol - begin);
			ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos); // Start of next shader type declaration line

			shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void OpenGLShader::IncludeLibs(std::string& source)
	{
		ENGINE_PROFILE_FUNCTION();
		size_t pos = source.find("#include");
		while (pos != std::string::npos)
		{
			size_t start = source.find("\"", pos);
			size_t end = source.find("\"", start + 1);
			std::string include = source.substr(start + 1, end - start - 1);
			std::string includeSource = *ReadFile("resources/shaders/" + include);
			source.replace(pos, end - pos + 1, includeSource);
			pos = source.find("#include");
		}
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		ENGINE_PROFILE_FUNCTION();

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetOptimizationLevel(shaderc_optimization_level_performance);
		options.SetGenerateDebugInfo();
		
		GLuint program = glCreateProgram();
		ASSERT(shaderSources.size() <= 2, "Only 2 shaders are currently supported (vertex, fragment)");
		std::array<GLenum, 2> glShaderIDs; // [0] = vertex, [1] = fragment
		int glShaderIDIndex = 0;

		for (auto& kv : shaderSources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;

			std::string sourceName = type == GL_VERTEX_SHADER ? "Vertex" : "Fragment";
			shaderc_shader_kind shaderType = type == GL_VERTEX_SHADER ? shaderc_glsl_vertex_shader : shaderc_glsl_fragment_shader;

			shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source, shaderType, sourceName.c_str(), options);

			if (result.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				LOG_ERROR("{0}", result.GetErrorMessage());
				ASSERT(false, "Shader compilation failure!");
			}

			std::vector<uint32_t> spirvBinary(result.cbegin(), result.cend());

			Reflect(sourceName, spirvBinary);

			// Create and bind the OpenGL shader
			GLuint shader = glCreateShader(type);
			glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, spirvBinary.data(), spirvBinary.size() * sizeof(uint32_t));

			// Specialize the shader (if needed, optional)
			glSpecializeShader(shader, "main", 0, nullptr, nullptr);

			// Attach the shader to the program
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
			{
				glDeleteShader(id);
			}

			LOG_ERROR("{0}", infoLog.data());
			ASSERT(false, "Shader link failure!");
			return;
		}

		for (auto id : glShaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}

		m_RendererID = program;
	}

	void OpenGLShader::Reflect(const std::string& shaderName, const std::vector<uint32_t>& spirv)
	{
		ENGINE_PROFILE_FUNCTION();


		spirv_cross::Compiler compiler(spirv);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		LOG_INFO("----- {0} -----", shaderName);

		// Reflect Input Variables
		if (shaderName == "Vertex") 
		{
			for (const auto& input : resources.stage_inputs)
			{
				std::string input_name = compiler.get_name(input.id);
				auto& input_type = compiler.get_type(input.base_type_id);
				uint32_t location = compiler.get_decoration(input.id, spv::DecorationLocation);

				LOG_TRACE("Input Variable: {0}, Location: {1}", input_name, location);

				ShaderDataType type = ShaderTypeFromSpv(input_type);

				m_Vertex.AddAttribute(input_name, type, location);
			}

			// Make sure to preserve the order of attributes (order by locations in shader)
			for(const auto& attribute : m_Vertex.GetAttributesOredered())
			{
				m_InputLayout.Push(attribute.type, attribute.name);
			}

		}
		

		// Reflect Uniform Buffers
		for (const auto& uniform_buffer : resources.uniform_buffers)
		{
			std::string buffer_name = compiler.get_name(uniform_buffer.id);
			auto& buffer_type = compiler.get_type(uniform_buffer.base_type_id);

			LOG_TRACE("Uniform Buffer: {0} {1}", buffer_name, buffer_type.array.size());

			for (uint32_t i = 0; i < buffer_type.member_types.size(); i++)
			{
				auto& memberType = compiler.get_type(buffer_type.member_types[i]);
				std::string memberName = compiler.get_member_name(uniform_buffer.base_type_id, i);

				LOG_TRACE("  Member: {0} {1}, columns: {2}, vecsize: {3}", memberName, (int)memberType.basetype, memberType.columns, memberType.vecsize);
				ShaderDataType type = ShaderTypeFromSpv(memberType);

				if (buffer_name == "u_Material")
				{
					m_MaterialLayout.Push(type, memberName);
				}
				else if (buffer_name == "u_Global") 
				{

				}
				else
				{
					LOG_WARNING("Unknown uniform buffer: {0}", buffer_name);
					//m_ObjectLayout.Push(type, memberName);
				}
			}
		}
	}



	//TODO: use predefined map
	ShaderDataType OpenGLShader::ShaderTypeFromSpv(spirv_cross::SPIRType spvType)
	{
		switch (spvType.basetype)
		{
		case spirv_cross::SPIRType::Float:
			if (spvType.columns == 4 && spvType.vecsize == 4)
			{
				return ShaderDataType::Mat4;
			}
			else if (spvType.columns == 3 && spvType.vecsize == 3)
			{
				return ShaderDataType::Mat3;
			}
			else if (spvType.columns == 1 && spvType.vecsize == 4)
			{
				return ShaderDataType::Float4;
			}
			else if (spvType.columns == 1 && spvType.vecsize == 3)
			{
				return ShaderDataType::Float3;
			}
			else if (spvType.columns == 1 && spvType.vecsize == 2)
			{
				return ShaderDataType::Float2;
			}
			else
			{
				return ShaderDataType::Float;
			}
			
		case spirv_cross::SPIRType::Int:
			if (spvType.columns == 4 && spvType.vecsize == 4)
			{
				return ShaderDataType::Int4;
			}
			else if (spvType.columns == 3 && spvType.vecsize == 3)
			{
				return ShaderDataType::Int3;
			}
			else if (spvType.columns == 2 && spvType.vecsize == 2)
			{
				return ShaderDataType::Int2;
			}
			else
			{
				return ShaderDataType::Int;
			}
		case spirv_cross::SPIRType::Boolean:
			return ShaderDataType::Bool;


		//default:
		//	ASSERT(false, "Unknown type!");
		}
		
	}


	OpenGLShader::~OpenGLShader()
	{
		ENGINE_PROFILE_FUNCTION();
		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		UploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		UploadUniformFloat4(name, value);
	}


	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		UploadUniformMat4f(name, value);
	}

	void OpenGLShader::Bind() const
	{
		ENGINE_PROFILE_FUNCTION();
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		ENGINE_PROFILE_FUNCTION();
		glUseProgram(0);
	}



	void OpenGLShader::UploadUniformInt(const std::string& name, int value)
	{
		glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		glUniform1iv(glGetUniformLocation(m_RendererID, name.c_str()), count, values);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
	{
		glUniform2f(glGetUniformLocation(m_RendererID, name.c_str()), values.x, values.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
	{
		glUniform3f(glGetUniformLocation(m_RendererID, name.c_str()), values.x, values.y, values.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values)
	{
		glUniform4f(glGetUniformLocation(m_RendererID, name.c_str()), values.x, values.y, values.z, values.w);
	}

	void OpenGLShader::UploadUniformMat3f(const std::string& name, const glm::mat3& matrix)
	{
		glUniformMatrix3fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	}
	void OpenGLShader::UploadUniformMat4f(const std::string& name, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	}

}