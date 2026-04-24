#include "enginepch.h"
#include "Engine/Core/UUID.h"
#include <random>

namespace Engine
{
	Uuid::Uuid()
	{
		thread_local std::random_device rd;
		thread_local std::mt19937_64 gen(rd());
		thread_local std::uniform_int_distribution<uint64_t> dis;

		m_Value = 0;
		do { m_Value = dis(gen); } 
		while (m_Value <= 1000); // reserve low values for special cases
	}
}