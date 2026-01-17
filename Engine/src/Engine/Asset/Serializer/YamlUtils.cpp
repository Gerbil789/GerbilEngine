#include "enginepch.h"
#include "YamlUtils.h"

namespace Engine::Yaml
{
  void Write(YAML::Emitter& out, const std::string& key, const glm::vec3& v)
  {
    out << YAML::Key << key << YAML::Value;
    Seq seq(out, true);
    out << v.x << v.y << v.z;
  }

  void Write(YAML::Emitter& out, const std::string& key, const glm::vec4& v)
  {
    out << YAML::Key << key << YAML::Value;
    Seq seq(out, true);
    out << v.x << v.y << v.z << v.w;
  }

  bool Read(const YAML::Node& node, glm::vec3& v)
  {
    if (!node.IsSequence() || node.size() != 3) return false;
    v = { node[0].as<float>(), node[1].as<float>(), node[2].as<float>() };
    return true;
  }

  bool Read(const YAML::Node& node, glm::vec4& v)
  {
    if (!node.IsSequence() || node.size() != 4) return false;
    v = {
        node[0].as<float>(),
        node[1].as<float>(),
        node[2].as<float>(),
        node[3].as<float>()
    };
    return true;
  }
}
