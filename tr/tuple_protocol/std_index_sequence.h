#pragma once

#include "../tuple_protocol.h"
#include "../value_constant.h"

#include <cstddef>
#include <utility>

namespace tr
{
	template <std::size_t ... Is>
	struct tup_size<std::index_sequence<Is...>>
		: std::integral_constant<std::size_t, sizeof...(Is)> {};

	template <std::size_t J, std::size_t ... Is>
	[[nodiscard]] constexpr auto get(std::index_sequence<Is...>) noexcept
	{
		static_assert(J < sizeof...(Is), "Index out of bounds");
		constexpr std::size_t indices[]{ Is... };
		return value_c<indices[J]>;
	}
}