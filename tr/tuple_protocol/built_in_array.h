#pragma once

#include "../detail/utility.h"
#include "../tuple_protocol.h"

#include <cstddef>
#include <type_traits>

namespace tr
{
	template <typename T, std::size_t N>
	struct tup_size<T[N]>
		: std::integral_constant<std::size_t, N> {};

	template <typename T, std::size_t N>
	struct tup_size<T const[N]>
		: std::integral_constant<std::size_t, N> {};

	namespace detail
	{
		template <std::size_t I, typename T>
		constexpr decltype(auto) array_get_impl(T&& arr) noexcept
		{
			using array_t = std::remove_reference_t<T>;
			static_assert(std::is_array_v<array_t>, "T is not a reference to a built-in array");

			auto const elemCount = std::extent_v<array_t>;
			static_assert(I < elemCount, "Index out of bounds");
			return forward_like<T>(arr[I]);
		}
	}

	template <std::size_t I, typename T, std::size_t N>
	[[nodiscard]] constexpr T const& get(T const (&arr)[N]) noexcept {
		return detail::array_get_impl<I>(arr);
	}

	template <std::size_t I, typename T, std::size_t N>
	[[nodiscard]] constexpr T& get(T(&arr)[N]) noexcept {
		return detail::array_get_impl<I>(arr);
	}

	template <std::size_t I, typename T, std::size_t N>
	[[nodiscard]] constexpr T&& get(T(&&arr)[N]) noexcept {
		return detail::array_get_impl<I>(std::move(arr));
	}
}