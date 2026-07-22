#include "enginepch.h"
#include "Engine/Core/UUID.h"
#include <random>

namespace Engine
{
	Uuid Uuid::Generate()
	{
		thread_local std::random_device rd;
		thread_local std::mt19937_64 gen(rd());
		thread_local std::uniform_int_distribution<uint64_t> dis;

		uint64_t value = 0;
		do { value = dis(gen); } 
		while (value <= 1000); // reserve low values for special cases

		return Uuid{ value };
	}
}