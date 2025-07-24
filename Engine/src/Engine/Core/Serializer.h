#pragma once

namespace Engine
{
	class Project;
	class Scene;
	class Material;
}

namespace Engine::Serializer
{
	void Serialize(Scene* scene);
	bool Deserialize(Scene* scene);

	void Serialize(const Ref<Material>& material);
	bool Deserialize(Ref<Material>& material);
}