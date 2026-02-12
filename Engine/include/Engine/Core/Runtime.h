#pragma once

#include "Engine/Core/API.h"
#include "Engine/Core/GameInstance.h"

namespace Engine
{
	class ENGINE_API Runtime
	{
	public:
		static void Start();
		static void Stop();
		static void Update(float delta);

	private:
		inline static std::unique_ptr<GameInstance> m_GameInstance = nullptr;
	};
}