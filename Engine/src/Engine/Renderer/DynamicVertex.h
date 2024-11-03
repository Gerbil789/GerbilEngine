#pragma once

#include "Engine/Renderer/Buffer.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <cstdint>
#include <unordered_map>

namespace Engine 
{
  struct VertexAttribute 
  {
    ShaderDataType type;
    size_t offset;
    std::string name;
  };

  class DynamicVertex 
  {
  public:
    void AddAttribute(const std::string& name, ShaderDataType type);

    template<typename T>
    void SetAttribute(const std::string& name, const T& value) 
    {
      auto it = m_attributes.find(name);
      if (it == m_attributes.end()) 
      {
				LOG_ERROR("Vertex Attribute not found: {0}", name);
        return;
      }
      

      const VertexAttribute& attr = it->second;
      std::memcpy(m_data.data() + attr.offset, &value, sizeof(T));
    }

    template<typename T>
    T GetAttribute(const std::string& name) const {
      auto it = m_attributes.find(name);
      if (it == m_attributes.end())
      {
        LOG_ERROR("Vertex Attribute not found: {0}", name);
        return;
      }

      const VertexAttribute& attr = it->second;
      T value;
      std::memcpy(&value, m_data.data() + attr.offset, sizeof(T));
      return value;
    }



    const void* GetData() const { return m_data.data(); }
    size_t GetSize() const { return m_data.size(); }

  private:
    std::vector<uint8_t> m_data;
    std::unordered_map<std::string, VertexAttribute> m_attributes;
  };

}