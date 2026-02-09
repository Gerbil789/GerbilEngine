#pragma once

#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

namespace Engine::Yaml
{
  // ------------------------------------------------------------
  // RAII scopes
  // ------------------------------------------------------------

  struct Map
  {
    YAML::Emitter& out;
    explicit Map(YAML::Emitter& e) : out(e) { out << YAML::BeginMap; }
    ~Map() { out << YAML::EndMap; }
  };

  struct Seq
  {
    YAML::Emitter& out;
    explicit Seq(YAML::Emitter& e, bool flow = false) : out(e)
    {
      if (flow) out << YAML::Flow;
      out << YAML::BeginSeq;
    }
    ~Seq() { out << YAML::EndSeq; }
  };

  // ------------------------------------------------------------
  // Write helpers
  // ------------------------------------------------------------

  template<typename T>
  void Write(YAML::Emitter& out, const std::string& key, const T& value)
  {
    out << YAML::Key << key << YAML::Value << value;
  }

  void Write(YAML::Emitter& out, const std::string& key, const glm::vec3& v);
  void Write(YAML::Emitter& out, const std::string& key, const glm::vec4& v);

  // ------------------------------------------------------------
  // Read helpers
  // ------------------------------------------------------------

  template<typename T>
  bool Read(const YAML::Node& node, const std::string& key, T& out)
  {
    auto n = node[key];
    if (!n) return false;
    out = n.as<T>();
    return true;
  }

  bool Read(const YAML::Node& node, glm::vec3& out);
  bool Read(const YAML::Node& node, glm::vec4& out);
}
