#include "enginepch.h"
#include "DynamicVertex.h"
#include "Engine/Core/Log.h"

namespace Engine 
{
	void DynamicVertex::AddAttribute(const std::string& name, ShaderDataType type, uint32_t location)
	{
		size_t offset = m_data.size();
		size_t size = ShaderDataTypeSize(type);

		m_attributes[name] = { type, offset, name, location };
		m_data.resize(m_data.size() + size);
	}
}