#pragma once

#include <xhash>

namespace Engine 
{
	class UUID //TODO: use uint64_t
	{
	public:
		UUID();
		UUID(uint32_t uuid) : m_UUID(uuid) {}
		UUID(const UUID& other) = default;

		operator uint32_t() const { return m_UUID; }
	private:
		uint32_t m_UUID;
	};
}

namespace std
{
	template<>
	struct hash<Engine::UUID>
	{
		size_t operator()(const Engine::UUID& uuid) const
		{
			return hash<uint32_t>()((uint32_t)uuid);
		}
	};
}