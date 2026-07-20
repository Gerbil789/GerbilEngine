#include "enginepch.h"
#include "Engine/Asset/Serializer/ShaderParser.h"
#include "Engine/Graphics/WebGPUUtils.h"
#include <regex>
#include <utility>

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
		if (type == "vec2i")     return ShaderValueType::Vec2i;
		if (type == "mat3x3f")   return ShaderValueType::Mat3;
		if (type == "mat4x4f")   return ShaderValueType::Mat4;

		if (type == "array<mat4x4f") return ShaderValueType::ArrayMat4;
		if (type == "array<f32") return ShaderValueType::ArrayFloat;

		// fallback
		LOG_WARNING("Unknown type: {}", type);
		return ShaderValueType::Float;
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
			case ShaderValueType::ArrayMat4: return 64 * 4; //TODO: dont hardcode array size, need to parse it from shader source
			case ShaderValueType::ArrayFloat: return 4 * 4; //TODO: dont hardcode array size, need to parse it from shader source
		}

		std::unreachable();
	}


	//static void ParseParameterAttributes(ShaderParameter& param, const std::string& attributes)
	//{
	//	param.isColor = attributes.find("@color") != std::string::npos;

	//	std::smatch match;
	//	static const std::regex defaultRegex(R"(@default\(([^)]*)\))");

	//	if (std::regex_search(attributes, match, defaultRegex))
	//	{
	//		std::vector<float> nums;
	//		std::stringstream ss(match[1].str());
	//		std::string token;
	//		while (std::getline(ss, token, ','))
	//		{
	//			nums.push_back(std::stof(token));
	//		}

	//		switch (param.type)
	//		{
	//			case ShaderValueType::Float: param.defaultValue = nums[0]; break;
	//			case ShaderValueType::Vec2:  param.defaultValue = glm::vec2{nums[0], nums[1]}; break;
	//			case ShaderValueType::Vec3:  param.defaultValue = glm::vec3{nums[0], nums[1], nums[2]}; break;
	//			case ShaderValueType::Vec4:  param.defaultValue = glm::vec4{nums[0], nums[1], nums[2], nums[3]}; break;
	//		}
	//	}
	//}

	struct StructMember
	{
		std::string name;
		std::string type;
		std::string comment;
	};

	struct ShaderStruct
	{
		std::string name;
		std::vector<StructMember> members;
	};

	// Helper function to trim whitespace from both ends of a string
	std::string trim(const std::string& str)
	{
		size_t first = str.find_first_not_of(" \t\r\n");
		if (first == std::string::npos) return "";
		size_t last = str.find_last_not_of(" \t\r\n");
		return str.substr(first, (last - first + 1));
	}


	//static void ParseVertexInputs(const std::string& source, ShaderSpecification& spec)
	//{
	//	std::regex vertexInputRegex(R"(struct\s+VertexInput\s*\{([^}]*)\})");
	//	std::smatch inputMatch;
	//	if (!std::regex_search(source, inputMatch, vertexInputRegex))
	//	{
	//		return; // no vertex input
	//	}

	//	std::string body = inputMatch[1].str();

	//	std::regex attrRegex(R"(@location\((\d+)\)\s+(\w+)\s*:\s*([a-zA-Z0-9_]+))");
	//	std::smatch match;
	//	auto begin = body.cbegin();
	//	auto end = body.cend();

	//	while (std::regex_search(begin, end, match, attrRegex))
	//	{
	//		wgpu::VertexAttribute attrib;
	//		attrib.shaderLocation = std::stoi(match[1].str());
	//		attrib.format = StringToVertexFormat(match[3].str());
	//		spec.vertexAttributes.push_back(attrib);
	//		begin = match.suffix().first;
	//	}
	//}

	static std::vector<ShaderStruct> ParseWGSLStructs(const std::string& shaderCode)
	{
		std::vector<ShaderStruct> structs;
		std::istringstream stream(shaderCode);
		std::string line;

		bool insideStruct = false;
		ShaderStruct currentStruct;

		while (std::getline(stream, line))
		{
			std::string trimmed = trim(line);
			if (trimmed.empty()) continue;

			// detect struct start
			if (!insideStruct)
			{
				if (trimmed.find("struct ") == 0)
				{
					insideStruct = true;
					constexpr size_t nameStart = std::string_view{ "struct " }.size();
					size_t nameEnd = trimmed.find_first_of(" {", nameStart);
					currentStruct.name = trim(trimmed.substr(nameStart, nameEnd - nameStart));
					currentStruct.members.clear();
				}
				continue;
			}

			// detect struct end
			if (trimmed.find("}") == 0)
			{
				structs.push_back(currentStruct);
				insideStruct = false;
				continue;
			}

			// parse struct members
			StructMember member;

			// extract and remove comment
			size_t commentPos = trimmed.find("//");
			if (commentPos != std::string::npos)
			{
				member.comment = trim(trimmed.substr(commentPos + 2));
				trimmed = trimmed.substr(0, commentPos);
			}

			// find the colon separating name and type
			size_t colonPos = trimmed.find(':');
			if (colonPos == std::string::npos) continue; // Skip lines without a colon (e.g., just '{')

			// find the trailing comma
			size_t commaPos = trimmed.find_last_of(',');
			if (commaPos == std::string::npos || commaPos < colonPos)
			{
				commaPos = trimmed.length(); // fallback if the last member lacks a comma
			}

			// extract type (everything between : and ,)
			member.type = trim(trimmed.substr(colonPos + 1, commaPos - colonPos - 1));

			// extract name (the word immediately preceding the colon)
			std::string beforeColon = trim(trimmed.substr(0, colonPos));
			size_t lastSpaceOrParen = beforeColon.find_last_of(" )"); // Handles "@builtin(position) position"

			if (lastSpaceOrParen != std::string::npos)
			{
				member.name = trim(beforeColon.substr(lastSpaceOrParen + 1));
			}
			else
			{
				member.name = beforeColon;
			}

			if (!member.name.empty() && !member.type.empty())
			{
				currentStruct.members.push_back(member);
			}
		}

		return structs;
	}

	//static std::unordered_map<std::string, std::vector<ShaderParameter>> ParseStructs(const std::string& source)
	//{
	//	std::unordered_map<std::string, std::vector<ShaderParameter>> structs;

	//	std::regex structRegex(R"(struct\s+(\w+)\s*\{([^}]*)\})");
	//	std::smatch match;
	//	auto begin = source.cbegin();
	//	auto end = source.cend();

	//	while (std::regex_search(begin, end, match, structRegex))
	//	{
	//		std::string structName = match[1].str();
	//		std::string body = match[2].str();

	//		std::vector<ShaderParameter> params;
	//		size_t offset = 0;

	//		std::regex memberRegex(R"(\s*(\w+)\s*:\s*([^,\n}]+)\s*,?\s*(?://(.*))?)");

	//		std::smatch memberMatch;
	//		auto mbegin = body.cbegin();
	//		auto mend = body.cend();

	//		while (std::regex_search(mbegin, mend, memberMatch, memberRegex))
	//		{
	//			ShaderParameter param;
	//			param.name = memberMatch[1].str();
	//			param.type = ParseValueType(memberMatch[2].str());
	//			ParseParameterAttributes(param, memberMatch[3].str());
	//			param.offset = offset;
	//			param.size = GetTypeSize(param.type);

	//			params.push_back(param);

	//			offset += param.size;
	//			mbegin = memberMatch.suffix().first;
	//		}

	//		structs[structName] = params;
	//		begin = match.suffix().first;
	//	}

	//	return structs;
	//}

	static void ParseBindings(const std::string& source, const std::unordered_map<std::string, std::vector<ShaderParameter>>& structs, ShaderSpecification& spec)
	{
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

		std::regex bindRegex(R"(@group\((\d+)\)\s*@binding\((\d+)\)\s*var(?:<\s*([^>]+?)\s*>)?\s+(\w+)\s*:\s*([^;\n]+))");
		begin = source.cbegin();

		while (std::regex_search(begin, end, match, bindRegex))
		{
			Binding binding;
			binding.group = std::stoi(match[1].str());
			binding.binding = std::stoi(match[2].str());
			std::string varKind = match[3].str(); // "uniform", "storage", empty
			binding.name = match[4].str();
			std::string typeName = match[5].str();

			if (varKind == "uniform")
				binding.type = BindingType::Uniform;
			else if (varKind == "storage")
				binding.type = BindingType::Storage;
			else if (typeName.rfind("texture_2d", 0) == 0)
				binding.type = BindingType::Texture2D;
			else if (typeName.rfind("texture_cube", 0) == 0)
				binding.type = BindingType::TextureCube;
			else if (typeName == "sampler")
				binding.type = BindingType::Sampler;

			auto it = structs.find(typeName);
			if (it != structs.end())
				binding.parameters = it->second;

			binding.size = 0;
			for (const ShaderParameter& param : binding.parameters)
			{
				binding.size += param.size;
			}

			for (const EntryPoint& ep : entryPoints)
			{
				std::regex usageRegex("\\b" + binding.name + R"([\.\[])", std::regex::optimize);
				if (std::regex_search(source, usageRegex))
				{
					binding.visibility = ep.stage;
				}
			}

			spec.bindings.push_back(binding);
			begin = match.suffix().first;
		}
	}

	ShaderSpecification ShaderParser::GetSpecification(const std::string& source)
	{
		if (source.empty())
		{
			LOG_ERROR("Shader source is empty!");
			return ShaderSpecification{};
		}

		ShaderSpecification spec;
		//ParseVertexInputs(source, spec);
		std::vector<ShaderStruct> structs = ParseWGSLStructs(source);
		ParseBindings(source, structs, spec);

		return spec;
	}
}