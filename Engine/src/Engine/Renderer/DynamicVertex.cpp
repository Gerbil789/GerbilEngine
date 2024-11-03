#include "enginepch.h"
#include "DynamicVertex.h"

namespace Engine 
{
	void DynamicVertex::AddAttribute(const std::string& name, ShaderDataType type)
	{
		size_t offset = m_data.size();
		size_t size = ShaderDataTypeSize(type);

		m_attributes[name] = { type, offset, name };
		m_data.resize(m_data.size() + size);
	}
}