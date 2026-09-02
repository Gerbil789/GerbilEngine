#pragma once

#include <cstdint>
#include <functional>
#include <compare>

namespace engine
{
	class Uuid
	{
	public:
		constexpr Uuid() = default;
		constexpr Uuid(uint64_t value) : m_Value(value) {}

		static Uuid Generate();

		auto operator<=>(const Uuid&) const = default;

		explicit operator uint64_t() const { return m_Value; }
		operator bool() const { return m_Value != 0; }

	private:
		uint64_t m_Value = 0;
	};
}

namespace std
{
	template<>
	struct hash<engine::Uuid>
	{
		size_t operator()(const engine::Uuid& uuid) const noexcept
		{
			return static_cast<size_t>(uuid);
		}
	};
}