#include "enginepch.h"
#include "Engine/Asset/Serializer/ShaderParser.h"
#include "Engine/Graphics/Utility.h"
#include <regex>

namespace Engine
{
  namespace
  {
    std::unordered_map<std::string, std::vector<ShaderParameter>> m_Structs;
    std::unordered_map<std::string, uint32_t> m_Constants;
  }

  static wgpu::VertexFormat StringToVertexFormat(const std::string& str)
  {
    static const std::unordered_map<std::string, wgpu::VertexFormat> formatMap = {
      { "vec1f", wgpu::VertexFormat::Float32 },
      { "vec2f", wgpu::VertexFormat::Float32x2 },
      { "vec3f", wgpu::VertexFormat::Float32x3 },
      { "vec4f", wgpu::VertexFormat::Float32x4 },

      { "vec2h", wgpu::VertexFormat::Float16x2 },
      { "vec4h", wgpu::VertexFormat::Float16x4 },

      { "vec1i", wgpu::VertexFormat::Sint32 },
      { "vec2i", wgpu::VertexFormat::Sint32x2 },
      { "vec3i", wgpu::VertexFormat::Sint32x3 },
      { "vec4i", wgpu::VertexFormat::Sint32x4 },

      { "vec1u", wgpu::VertexFormat::Uint32 },
      { "vec2u", wgpu::VertexFormat::Uint32x2 },
      { "vec3u", wgpu::VertexFormat::Uint32x3 },
      { "vec4u", wgpu::VertexFormat::Uint32x4 },

      { "vec2s", wgpu::VertexFormat::Sint16x2 },
      { "vec4s", wgpu::VertexFormat::Sint16x4 },

      { "vec2us", wgpu::VertexFormat::Uint16x2 },
      { "vec4us", wgpu::VertexFormat::Uint16x4 },

      { "vec2b", wgpu::VertexFormat::Sint8x2 },
      { "vec4b", wgpu::VertexFormat::Sint8x4 },

      { "vec2ub", wgpu::VertexFormat::Uint8x2 },
      { "vec4ub", wgpu::VertexFormat::Uint8x4 },
    };

    auto it = formatMap.find(str);
    if (it != formatMap.end())
      return it->second;

    LOG_ERROR("Unknown vertex attribute format: {}", str);
    return wgpu::VertexFormat::Float32;
  }

  void ParseValueType(const std::string& type, ShaderParameter& param)
  {
    if (type.starts_with("array<"))
    {
      uint32_t arraySize = 0;

      size_t start = type.find('<') + 1;
      size_t end = type.find('>');

      // Extract everything inside the < > (e.g., "vec3f" or "mat4x4f, SOME_CONST")
      std::string contents = type.substr(start, end - start);
      std::string innerType = contents;

      size_t commaPos = contents.find(',');
      if (commaPos != std::string::npos)
      {
        // Split by comma
        innerType = contents.substr(0, commaPos);
        std::string arraySizeStr = contents.substr(commaPos + 1);

        // Strip any whitespace so std::isdigit doesn't fail on something like " 47"
        std::erase_if(arraySizeStr, [](unsigned char c) { return std::isspace(c); });

        // Clean up innerType just in case there's whitespace (e.g., " vec3f ")
        std::erase_if(innerType, [](unsigned char c) { return std::isspace(c); });

        if (!arraySizeStr.empty())
        {
          if (std::isdigit(arraySizeStr[0]))
          {
            arraySize = std::stoul(arraySizeStr);
          }
          else
          {
            auto it = m_Constants.find(arraySizeStr);
            if (it != m_Constants.end())
            {
              arraySize = it->second;
            }
            else
            {
              LOG_WARNING("ParseValueType - Unknown constant: {}", arraySizeStr);
            }
          }
        }
      }

      // Recursively parse the inner type
      ParseValueType(innerType, param);
      param.arraySize = arraySize;

      return;
    }

    if (type == "f32") { param.type = ShaderValueType::Float; return; }
    if (type == "i32") { param.type = ShaderValueType::Int; return; }
    if (type == "u32") { param.type = ShaderValueType::UInt; return; }
    if (type == "bool") { param.type = ShaderValueType::Bool; return; }
    if (type == "vec2f") { param.type = ShaderValueType::Vec2; return; }
    if (type == "vec3f") { param.type = ShaderValueType::Vec3; return; }
    if (type == "vec4f") { param.type = ShaderValueType::Vec4; return; }
    if (type == "vec2i") { param.type = ShaderValueType::Vec2i; return; }
    if (type == "mat3x3f") { param.type = ShaderValueType::Mat3; return; }
    if (type == "mat4x4f") { param.type = ShaderValueType::Mat4; return; }

    LOG_WARNING("ParseValueType - Unknown type: {}", type);
    param.type = ShaderValueType::Float;
  }

  constexpr size_t GetTypeSize(ShaderValueType type)
  {
    switch (type)
    {
    case ShaderValueType::Bool:
    case ShaderValueType::Int:
    case ShaderValueType::UInt:
    case ShaderValueType::Float: return 4;
    case ShaderValueType::Vec2: return 8;
    case ShaderValueType::Vec3: return 12;
    case ShaderValueType::Vec4: return 16;
    case ShaderValueType::Vec2i: return 8;
    case ShaderValueType::Mat3: return 48;
    case ShaderValueType::Mat4: return 64;
    }
    std::unreachable();
  }

  void ParseConstants(std::string_view source)
  {
		m_Constants.clear();

    std::regex constRegex(R"(const\s+(\w+)\s*:\s*u32\s*=\s*(\d+))");
    std::cmatch match;

    const char* begin = source.data();
    const char* end = source.data() + source.size();

    while (std::regex_search(begin, end, match, constRegex))
    {
      std::string_view name(match[1].first, match[1].length());
      std::string_view valueStr(match[2].first, match[2].length());
      uint32_t value = std::stoul(std::string(valueStr));
			m_Constants.insert({ std::string(name), value });
      begin = match.suffix().first;
    }
	}

  void ParseParameterAttributes(ShaderParameter& param, std::string_view attributes)
  {
    // @default
    {
      std::cmatch match;
      std::regex regex(R"(@default\(([^)]*)\))");

			const char* begin = attributes.data();
			const char* end = attributes.data() + attributes.size();

      if (std::regex_search(begin, end, match, regex))
      {
        std::string values = match[1].str();

        std::vector<float> nums;
        std::stringstream ss(values);
        std::string token;

        while (std::getline(ss, token, ','))
        {
          nums.push_back(std::stof(token));
        }

        switch (param.type)
        {
        case ShaderValueType::Float:
					param.defaultValue.emplace<float>(nums[0]);
          break;

        case ShaderValueType::Vec2:
					param.defaultValue.emplace<glm::vec2>(nums[0], nums[1]);
          break;

        case ShaderValueType::Vec3:
					param.defaultValue.emplace<glm::vec3>(nums[0], nums[1], nums[2]);
          break;

        case ShaderValueType::Vec4:
					param.defaultValue.emplace<glm::vec4>(nums[0], nums[1], nums[2], nums[3]);

          break;
        }
      }
    }
   
    if (attributes.find("@color") != std::string::npos)
    {
			param.isColor = true;
    }
  }

  void ParseStruct(const std::cmatch& match)
  {
    std::vector<ShaderParameter> params;

    std::string_view structName(match[1].first, match[1].length());
    std::string_view body(match[2].first, match[2].length());
    size_t offset = 0;

    // 1: name, 2: type, 3: custom attributes
    std::regex regex(R"(\s*(\w+)\s*:\s*((?:\w+<[^>]+>|\w+))\s*,?\s*(?://(.*))?)");

    std::cmatch memberMatch;
    const char* begin = body.data();
    const char* end = body.data() + body.size();

    while (std::regex_search(begin, end, memberMatch, regex))
    {
      ShaderParameter param;
      param.name = memberMatch[1].str();

      ParseValueType(memberMatch[2].str(), param);

      ParseParameterAttributes(param, memberMatch[3].str());
      param.offset = offset;
      param.size = GetTypeSize(param.type);

      params.push_back(param);

      offset += param.size;
      begin = memberMatch.suffix().first;
    }

		m_Structs.insert({ std::string(structName), params });
  }

  void ParseStructs(std::string_view source)
  {
		m_Structs.clear();

    std::regex regex(R"(struct\s+(\w+)\s*\{([^}]*)\})");
    std::cmatch match;
    const char* begin = source.data();
    const char* end = source.data() + source.size();

    while (std::regex_search(begin, end, match, regex))
    {
			ParseStruct(match);
      begin = match.suffix().first;
    }
  }

  void ParseEntryPoints(std::string_view source, ShaderSpecification& spec)
  {
    std::regex entryRegex(R"(@(vertex|fragment|compute)\s+fn\s+(\w+))");
    std::cmatch match;

    const char* begin = source.data();
    const char* end = source.data() + source.size();

    while (std::regex_search(begin, end, match, entryRegex))
    {
      std::string_view stage(match[1].first, match[1].length());
      std::string_view fn(match[2].first, match[2].length());

      if (stage == "vertex")
      {
        spec.vsEntryPoint = fn;
      }
      else if (stage == "fragment")
      {
        spec.fsEntryPoint = fn;
      }

      begin = match.suffix().first;
    }
	}

  void ParseVertexInputs(std::string_view source, ShaderSpecification& spec)
  {
    // find entry point function
    const std::regex regex(std::format(R"(fn\s+{}\s*\(\s*in:\s*(\w+))", spec.vsEntryPoint), std::regex::optimize);
    std::cmatch match;

    if (!std::regex_search(source.data(), match, regex)) return; // no vertex input struct found

    const std::string& structName = match[1].str();

    // find struct definition
    //const std::regex structRegex(std::format(R"(struct\s+{}\s*\{([^}]*)\})", structName));
    const std::regex structRegex(R"(struct\s+)" + structName + R"(\s*\{([^}]*)\})");

    if (!std::regex_search(source.data(), match, structRegex))
    {
      LOG_ERROR("Vertex input struct '{}' not found in shader source.", structName);
      return;
    }

    //parse attributes
    std::string_view body(match[1].first, match[1].length());

    // 1: location, 2: type
    const std::regex attributeRegex(R"(@location\((\d+)\)\s+(?:\w+)\s*:\s*([a-zA-Z0-9_]+))");

    const char* begin = body.data();
    const char* end = body.data() + body.size();


		uint64_t currentOffset = 0;

    while (std::regex_search(begin, end, match, attributeRegex))
    {
      wgpu::VertexAttribute attrib;
      attrib.shaderLocation = std::stoi(match[1].str());
      attrib.format = StringToVertexFormat(match[2].str());

			uint64_t formatSize = GetVertexFormatSize(attrib.format);

			attrib.offset = currentOffset;
			currentOffset += formatSize;
      spec.vertexAttributes.push_back(attrib);
      begin = match.suffix().first;
    }
  }

  Binding ParseBindingMatch(const std::cmatch& match)
  {
    Binding binding;
    binding.group = std::stoi(match[1].str());
    binding.binding = std::stoi(match[2].str());
    binding.name = match[5].str();

    std::string_view addressSpace(match[3].first, match[3].length());
    std::string_view accessMode(match[4].first, match[4].length());
		std::string type = match[6].str();

    if(!addressSpace.empty())
    {
			BufferBinding bufferBinding;

      if(addressSpace == "uniform")
      {
        bufferBinding.type = wgpu::BufferBindingType::Uniform;
      }
      else if(addressSpace == "storage")
      {
				if (accessMode == "read" || accessMode.empty()) // default to read-only if not specified
        {
          bufferBinding.type = wgpu::BufferBindingType::ReadOnlyStorage;
        }
        else
        {
          bufferBinding.type = wgpu::BufferBindingType::Storage;
        }
			}
      else
      {
				LOG_WARNING("ParseBindingMatch - Unknown address space: {}", addressSpace);
      }

      if (type.starts_with("array<"))
      {
        uint32_t arraySize = 0;

				size_t start = type.find('<') + 1;
        size_t end = type.find('>');
        std::string contents = type.substr(start, end - start);
        std::string innerType = contents;
        size_t commaPos = contents.find(',');
        if (commaPos != std::string::npos)
        {
          innerType = contents.substr(0, commaPos);
          std::string arraySizeStr = contents.substr(commaPos + 1);
          std::erase_if(arraySizeStr, [](unsigned char c) { return std::isspace(c); });
          std::erase_if(innerType, [](unsigned char c) { return std::isspace(c); });
          if (!arraySizeStr.empty())
          {
            if (std::isdigit(arraySizeStr[0]))
            {
              arraySize = std::stoul(arraySizeStr);
            }
            else
            {
              auto it = m_Constants.find(arraySizeStr);
              if (it != m_Constants.end())
              {
                arraySize = it->second;
              }
              else
              {
                LOG_WARNING("ParseBindingMatch - Unknown constant: {}", arraySizeStr);
              }
            }
          }
        }
				bufferBinding.arraySize = arraySize;
        type = innerType;
      }

			auto it = m_Structs.find(type);
      if (it != m_Structs.end())
      {
        bufferBinding.parameters = it->second;
        bufferBinding.size = 0;
        for (const auto& param : bufferBinding.parameters)
        {
          bufferBinding.size += param.size;
        }
			}

			binding.data = bufferBinding;
    }
    else if (type.rfind("texture_2d", 0) == 0)
    {
			binding.data = TextureBinding{ wgpu::TextureViewDimension::e2D, wgpu::TextureSampleType::Float, false };
    }
    else if (type.rfind("texture_cube", 0) == 0)
    {
			binding.data = TextureBinding{ wgpu::TextureViewDimension::Cube, wgpu::TextureSampleType::Float, false };
    }
    else if(type == "sampler")
    {
			binding.data = SamplerBinding{ wgpu::SamplerBindingType::Filtering }; //TODO: do i need sampler info?
		}

    return binding;
  }


  void ParseBindings(std::string_view source, ShaderSpecification& spec)
  {
    // 1: group, 2: binding, 3: address space, 4: access mode, 5: name, 6: type
    std::regex regex(R"(@group\((\d+)\)\s*@binding\((\d+)\)\s*var(?:<(\w+)(?:\s*,\s*(\w+))?>)?\s+(\w+)\s*:\s*([a-zA-Z0-9_<>]+))");
    std::cmatch match;

    const char* begin = source.data();
    const char* end = source.data() + source.size();

    while (std::regex_search(begin, end, match, regex))
    {
      Binding binding = ParseBindingMatch(match);
      spec.bindings.push_back(binding);
      begin = match.suffix().first;
    }
  }

  bool ContainsIdentifier(std::string_view text, std::string_view identifier)
  {
    size_t pos = 0;

    while ((pos = text.find(identifier, pos)) != std::string_view::npos)
    {
      bool leftOk =
        pos == 0 ||
        !(std::isalnum(static_cast<unsigned char>(text[pos - 1])) ||
          text[pos - 1] == '_');

      size_t end = pos + identifier.size();

      bool rightOk =
        end == text.size() ||
        !(std::isalnum(static_cast<unsigned char>(text[end])) ||
          text[end] == '_');

      if (leftOk && rightOk)
        return true;

      ++pos;
    }

    return false;
  }

  ShaderSpecification ShaderParser::Parse(std::string_view source)
  {
    if (source.empty())
    {
      LOG_ERROR("ShaderParser::GetSpecification - Shader source is empty!");
      return ShaderSpecification{};
    }

    ParseConstants(source);
		ParseStructs(source);

    ShaderSpecification spec;
		ParseEntryPoints(source, spec);
    ParseVertexInputs(source, spec);
    ParseBindings(source, spec);

		// parse visibility for each binding based on entry points

    size_t pos = 0;

    while ((pos = source.find('@', pos)) != std::string::npos)
    {
			wgpu::ShaderStage stage = wgpu::ShaderStage::None;

      if (source.compare(pos, 7, "@vertex") == 0)
        stage = wgpu::ShaderStage::Vertex;
      else if (source.compare(pos, 9, "@fragment") == 0)
        stage = wgpu::ShaderStage::Fragment;
      else
      {
        ++pos;
        continue;
      }

      size_t openBrace = source.find('{', pos);
      if (openBrace == std::string::npos)
        break;

      int depth = 1;
      size_t end = openBrace + 1;

      while (end < source.size() && depth > 0)
      {
        if (source[end] == '{')
          ++depth;
        else if (source[end] == '}')
          --depth;

        ++end;
      }

      std::string_view functionBody(
        source.data() + openBrace + 1,
        end - openBrace - 2);

      // Search binding names in functionBody...

      for (auto& binding : spec.bindings)
      {
        if (ContainsIdentifier(functionBody, binding.name))
        {
          binding.visibility = stage;
        }
      }

      pos = end;
    }

    return spec;
  }
}