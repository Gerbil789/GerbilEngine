#include "enginepch.h"
#include "ShaderParser.h"
#include "Engine/Graphics/WebGPUUtils.h"

#include <regex>

namespace Engine
{
  ShaderValueType ParseValueType(const std::string& type)
  {
    if (type == "f32")       return ShaderValueType::Float;
    if (type == "i32")       return ShaderValueType::Int;
    if (type == "u32")       return ShaderValueType::UInt;
    if (type == "bool")      return ShaderValueType::Bool;
    if (type == "vec2f")     return ShaderValueType::Vec2;
    if (type == "vec3f")     return ShaderValueType::Vec3;
    if (type == "vec4f")     return ShaderValueType::Vec4;
    if (type == "mat3x3f")   return ShaderValueType::Mat3;
    if (type == "mat4x4f")   return ShaderValueType::Mat4;

    // fallback
		LOG_ERROR("ParseValueType - Unknown type string: {0}", type);
    return ShaderValueType::Float;
  }

  size_t GetTypeSize(ShaderValueType type)
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

    case ShaderValueType::Mat3: return 48;
    case ShaderValueType::Mat4: return 64;

    default: 
			LOG_ERROR("GetTypeSize - Unknown ShaderValueType!");
      return 4;
    }
  }



	ShaderSpecification ShaderParser::GetSpecification(const std::string& source)
	{
		if(source.empty())
		{
			LOG_ERROR("ShaderParser::GetSpecification - Shader source is empty!");
			return ShaderSpecification();
		}

    ShaderSpecification spec;
    ParseVertexInputs(source, spec);
    auto structs = ParseStructs(source);
    ParseBindings(source, structs, spec);

    return spec;
	}


  void ShaderParser::ParseVertexInputs(const std::string& source, ShaderSpecification& spec)
  {
    // First, extract the body of VertexInput only
    std::regex vertexInputRegex(R"(struct\s+VertexInput\s*\{([^}]*)\})");
    std::smatch inputMatch;
    if (!std::regex_search(source, inputMatch, vertexInputRegex))
      return; // no VertexInput found

    std::string body = inputMatch[1].str();

    // Now parse @location() attributes only inside VertexInput
    std::regex attrRegex(R"(@location\((\d+)\)\s+(\w+)\s*:\s*([a-zA-Z0-9_]+))");
    std::smatch match;
    auto begin = body.cbegin();
    auto end = body.cend();

    while (std::regex_search(begin, end, match, attrRegex))
    {
      VertexAttribute attrib;
      attrib.location = std::stoi(match[1].str());
      attrib.label = match[2].str();
      attrib.format = StringToVertexFormat(match[3].str());

      spec.vertexAttributes.push_back(attrib);
      begin = match.suffix().first;
    }
  }

  std::unordered_map<std::string, std::vector<ShaderParameter>> ShaderParser::ParseStructs(const std::string& source)
  {
    std::unordered_map<std::string, std::vector<ShaderParameter>> structs;

    std::regex structRegex(R"(struct\s+(\w+)\s*\{([^}]*)\})");
    std::smatch match;
    auto begin = source.cbegin();
    auto end = source.cend();

    while (std::regex_search(begin, end, match, structRegex))
    {
      std::string structName = match[1].str();
      std::string body = match[2].str();

      std::vector<ShaderParameter> params;
      size_t offset = 0;

      std::regex memberRegex(R"(\s*(\w+)\s*:\s*([a-zA-Z0-9_]+))");
      std::smatch memberMatch;
      auto mbegin = body.cbegin();
      auto mend = body.cend();

      while (std::regex_search(mbegin, mend, memberMatch, memberRegex))
      {
        ShaderParameter param;
        param.name = memberMatch[1].str();
        param.type = ParseValueType(memberMatch[2].str());
        param.offset = offset;
        param.size = GetTypeSize(param.type);
        params.push_back(param);

        offset += param.size;
        mbegin = memberMatch.suffix().first;
      }

      structs[structName] = params;
      begin = match.suffix().first;
    }

    return structs;
  }
  void ShaderParser::ParseBindings(const std::string& source, const std::unordered_map<std::string, std::vector<ShaderParameter>>& structs, ShaderSpecification& spec)
  {
    // --- Step 1: collect entry points ---
    struct EntryPoint { std::string name; wgpu::ShaderStage stage; };
    std::vector<EntryPoint> entryPoints;

    std::regex entryRegex(R"(@(vertex|fragment|compute)\s+fn\s+(\w+))");
    std::smatch match;
    auto begin = source.cbegin();
    auto end = source.cend();

    while (std::regex_search(begin, end, match, entryRegex))
    {
      std::string stageStr = match[1].str();
      std::string fnName = match[2].str();

      wgpu::ShaderStage stage = wgpu::ShaderStage::None;
      if (stageStr == "vertex")
      {
        stage = wgpu::ShaderStage::Vertex;
				spec.vsEntryPoint = fnName;
      }
      else if (stageStr == "fragment") 
      {
        stage = wgpu::ShaderStage::Fragment;
				spec.fsEntryPoint = fnName;
      } 
      else if (stageStr == "compute") 
      {
        stage = wgpu::ShaderStage::Compute;
      } 

      entryPoints.push_back({ fnName, stage });
      begin = match.suffix().first;
    }

    // --- Step 2: parse bindings ---
    std::regex bindRegex(R"(@group\((\d+)\)\s*@binding\((\d+)\)\s*var(?:<(\w+)>)?\s+(\w+)\s*:\s*([a-zA-Z0-9_<>]+))");
    begin = source.cbegin();

    while (std::regex_search(begin, end, match, bindRegex))
    {
      Binding binding;
      binding.group = std::stoi(match[1].str());
      binding.binding = std::stoi(match[2].str());
      std::string varKind = match[3].str(); // "uniform", "storage", empty
      binding.name = match[4].str();
      std::string typeName = match[5].str();

      // --- Determine type ---
      if (varKind == "uniform")
        binding.type = BindingType::UniformBuffer;
      else if (varKind == "storage")
        binding.type = BindingType::StorageBuffer;
      else if (typeName.rfind("texture", 0) == 0)
        binding.type = BindingType::Texture2D; // TODO: detect 2D/3D
      else if (typeName == "sampler")
        binding.type = BindingType::Sampler;

      // --- Expand struct parameters if needed ---
      auto it = structs.find(typeName);
      if (it != structs.end())
        binding.parameters = it->second;

			// --- Determine size ---
			binding.size = 0;
      for (const auto& param : binding.parameters)
				binding.size += param.size;


      // --- Step 3: determine visibility ---
      for (auto& ep : entryPoints)
      {
        std::regex usageRegex("\\b" + binding.name + R"([\.\[])", std::regex::optimize);
        if (std::regex_search(source, usageRegex))
        {
          binding.visibility = ep.stage;
          //binding.visibility = static_cast<wgpu::ShaderStage>(static_cast<uint32_t>(binding.visibility) | static_cast<uint32_t>(ep.stage));
        }
      }

      spec.bindings.push_back(binding);
      begin = match.suffix().first;
    }
  }
}