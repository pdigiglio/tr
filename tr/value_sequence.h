#pragma once

#include "detail/type_traits.h"
#include "value_constant.h"

#include <cstddef>
#include <type_traits>

namespace tr
{
	namespace detail
	{
		struct from_any {
			template <typename T>
			/* implicit */ constexpr from_any(T) noexcept /* undefined */;
		};

		template <std::size_t I, auto Val>
		struct value_sequence_elem
		{
			template <typename T>
			constexpr decltype(Val) operator[](std::integral_constant<T, I>) const noexcept
			{
				return Val;
			}
		};

		template <typename T, typename IdxPack, auto... Vals>
		struct value_sequence_impl;

		template <typename T, std::size_t ... Is, auto... Vals>
		struct value_sequence_impl<T, std::index_sequence<Is...>, Vals...>
			: value_sequence_elem<Is, Vals>...
		{
			static_assert(
				are_same_v<T, detail::from_any> || are_same_v<T, decltype(Vals)...>,
				"This instanciation is neither a value tuple nor a value array.");

			using value_sequence_elem<Is, Vals>::operator[]...;
		};
	}

	template <typename T, auto... Vals>
	using value_sequence = detail::value_sequence_impl<
		T, std::make_index_sequence<sizeof...(Vals)>, Vals...>;


	//template <typename T, auto... Vals>
	//struct value_sequence {
	//	static_assert(are_same_v<T, detail::from_any> || are_same_v<T, decltype(Vals)...>);
	//};

	//template <typename T, T... Vals>
	//struct value_sequence<T, Vals...>
	//	: detail::value_sequence_impl<T, std::make_index_sequence<sizeof...(Vals)>, Vals...>
	//{
	//	///// @brief Implicit cast to another tuple. This will cast both to:
	//	/////  - an homogeneous tuple whose type `U` is such that casting from `T` to
	//	/////  `U` doesn't narrow;
	//	/////  - an heterogeneous tuple;
	//	//template <typename U, typename U_ = if_<
	//	//	(common_type_c<T, U> == type_c<from_any>), T, U>>
	//	//	constexpr operator value_sequence<U, U_{ Vals }...>() const noexcept {
	//	//	return {};
	//	//}
	//};

	//template <auto... Vals>
	//struct value_sequence<detail::from_any, Vals...>
	//	: detail::value_sequence_impl<detail::from_any, std::make_index_sequence<sizeof...(Vals)>, Vals...>
	//{
	//	///// @brief Implicit cast to homogeneous tuple (if all of `Vals...` can be
	//	///// converted to `T` without narrowing).
	//	//template <typename T>
	//	//constexpr operator value_sequence<T, T{ Vals }...>() const noexcept {
	//	//	return {};
	//	//}
	//};

	template <std::size_t I, typename T, auto... Vals>
	[[nodiscard]] constexpr auto at_(value_sequence<T,  Vals...> vs) noexcept
	{
		static_assert(I < sizeof...(Vals));
		constexpr std::integral_constant<std::size_t, I> idx{};
		return vs[idx];

		//return tr::get<I>(tr::tuple{ Vals... });
	}


	template <typename T, auto... Vals0, typename U, auto... Vals1>
	constexpr auto operator+(value_sequence<T, Vals0...>, value_sequence<U, Vals1...>) noexcept
		-> value_sequence<std::common_type_t<T, U>, Vals0..., Vals1...> {
		return {};
	}

	template <typename T, auto... Vals0, auto Val>
	constexpr auto operator+(value_sequence<T, Vals0...>, value_constant<Val>) noexcept
		-> value_sequence<std::common_type_t<T, decltype(Val)>, Vals0 + Val...> {
		return {};
	}

	template <typename T, auto... Vals0, auto Val>
	constexpr auto operator+(value_constant<Val> val, value_sequence<T, Vals0...> tup) noexcept
		-> decltype(tup + val) {
		return {};
	}

	/// @brief A sequence of homogeneous values.
	template <typename T, T... Vals>
	using value_array_constant = value_sequence<T, Vals...>;

	template <typename T, T... Vals>
	static constexpr value_array_constant<T, Vals...> array_c{};

	/// @brief A sequence of (possibly) heterogeneous values.
	template <auto... Vals>
	using value_tuple_constant = value_sequence<detail::from_any, Vals...>;

	template <auto... Vals>
	static constexpr value_tuple_constant<Vals...> tuple_c{};
}