#include "enginepch.h"
#include "Engine/Core/UUID.h"
#include <random>

namespace Engine
{
	static uint64_t GenerateUUID()
	{
		static std::random_device rd;
		static std::mt19937_64 gen(rd());
		static std::uniform_int_distribution<uint64_t> dis;

		uint64_t value = 0;
		do
		{
			value = dis(gen);
		} while (value == 0); // reserve 0 as invalid

		return value;
	}

	Uuid::Uuid(): m_Value(GenerateUUID()){}
}