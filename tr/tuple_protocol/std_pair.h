#pragma once

#include "../tuple_protocol.h"

#include <utility>

namespace tr
{
	template <typename ... Ts>
	struct tup_size<std::pair<Ts...>>
		: std::tuple_size<std::pair<Ts...>> {};

	template <std::size_t I, typename ... Ts>
	struct tup_elem<I, std::pair<Ts...>>
		: std::tuple_element<I, ::std::pair<Ts...>> {};

	template <std::size_t I, typename... Ts>
	[[nodiscard]] constexpr decltype(auto) get(std::pair<Ts...> const& t) noexcept
	{
		return std::get<I>(t);
	}

	template <std::size_t I, typename... Ts>
	[[nodiscard]] constexpr decltype(auto) get(std::pair<Ts...>& t) noexcept
	{
		return std::get<I>(t);
	}

	template <std::size_t I, typename... Ts>
	[[nodiscard]] constexpr decltype(auto) get(std::pair<Ts...>&& t) noexcept
	{
		return std::get<I>(std::move(t));
	}
}
