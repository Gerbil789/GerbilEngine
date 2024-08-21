#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Core/Asset.h"
#include "Engine/Scene/Material.h"

namespace Engine
{
	class Serializer
	{
	public:
		static void Serialize(const Ref<Material>& material);
		static bool Deserialize(Ref<Material>& material);

		static void Serialize(const Ref<Scene>& scene);
		static bool Deserialize(Ref<Scene>& scene);

	};


}