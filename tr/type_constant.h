#pragma once

#include <type_traits>

namespace tr
{
	template <typename T>
	struct type_constant {
		using type = T;

		template <typename U>
		[[nodiscard]] friend constexpr bool operator==(type_constant, type_constant<U>) noexcept {
			return std::is_same_v<T, U>;
		}

		template <typename U>
		[[nodiscard]] friend constexpr bool operator!=(type_constant t, type_constant<U> u) noexcept {
			return !(t == u);
		}
	};

	template <typename T>
	type_constant(T&&) -> type_constant<T>;

	template <typename T>
	static constexpr type_constant<T> type_c{};

	//template <typename... Ts>
	//static constexpr type_constant<
	//	std::conditional_t<sizeof...(Ts), std::common_type_t<Ts...>, void>>
	//	common_type_c{};
}