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
		static void Serialize(const Ref<Material>& material, const std::string& filePath);
		static bool Deserialize(const std::string& filePath, Ref<Material>& material);


	};


}