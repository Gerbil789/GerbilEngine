#pragma once

#include "Engine/Graphics/Shader.h"

namespace Engine 
{
	class ShaderParser
	{
	public:
		static ShaderSpecification GetSpecification(const std::string& source);

	private:
		static void ParseVertexInputs(const std::string& source, ShaderSpecification& spec);
		static std::unordered_map<std::string, std::vector<ShaderParameter>> ParseStructs(const std::string& source);
		static void ParseBindings(const std::string& source, const std::unordered_map<std::string, std::vector<ShaderParameter>>& structs, ShaderSpecification& spec);
	};
}