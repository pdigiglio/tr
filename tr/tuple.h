#pragma once

#include "./detail/ebo.h"
#include "./detail/type_pack.h"
#include "./detail/type_traits.h"
#include "tuple_protocol.h"

#include <cstddef>
#include <utility>

namespace tr
{
	namespace detail
	{
		template <std::size_t>
		struct tuple_tag;

		template <typename T, std::size_t I>
		struct tup_elem : ebo<T, tuple_tag<I>>
		{
			template <typename Int>
			constexpr T const& operator[](std::integral_constant<Int, I> ic) const& noexcept
			{
				return tup_elem::subscript_impl(*this, ic);
			}

			template <typename Int>
			constexpr T& operator[](std::integral_constant<Int, I> ic)& noexcept
			{
				return tup_elem::subscript_impl(*this, ic);
			}

			template <typename Int>
			constexpr T const&& operator[](std::integral_constant<Int, I> ic) const&& noexcept
			{
				return tup_elem::subscript_impl(std::move(*this), ic);
			}

			template <typename Int>
			constexpr T&& operator[](std::integral_constant<Int, I> ic)&& noexcept
			{
				return tup_elem::subscript_impl(std::move(*this), ic);
			}

			///// @brief An undefined member function to query the type of this
			///// tup_elem in traits like std::tuple_element.
			/////
			///// @tparam J An index that't only used to match it against \c I.
			/////
			///// @return `type_identity<T>`. If I return `T` directly, I can't
			///// use this technique to query the type of a `tup_elem<T[N]>`, as
			///// I can't return arrays.
			//template <std::size_t J, typename = std::enable_if_t<I == J>>
			//static type_identity<T> get_type() /* undefined */;


			/// @brief An undefined member function to query the type of this
			/// tup_elem in traits like std::tuple_element.
			///
			/// @return `type_identity<T>`. If I return `T` directly, I can't
			/// use this technique to query the type of a `tup_elem<T[N]>`, as
			/// I can't return arrays.
			static type_identity<T> get_type(std::integral_constant<std::size_t, I>) /* undefined */;

		private:
			template <
				typename TupElem,
				typename Int,
				typename = std::enable_if_t<std::is_integral_v<Int>>>
			constexpr static decltype(auto) subscript_impl(TupElem&& tupElem, std::integral_constant<Int, I>) noexcept
			{
				using ebo_t = ebo<T, tuple_tag<I>>;
				return get_ebo_val(forward_as<ebo_t, TupElem>(tupElem));
			}
		};

		template <typename TL, typename IL>
		struct tuple_base;

		template <typename... Ts, std::size_t... Is>
		struct tuple_base<type_pack<Ts...>, std::index_sequence<Is...>>
			: tup_elem<Ts, Is>...
		{
			using tup_elem<Ts, Is>::operator[]...;
			using tup_elem<Ts, Is>::get_type...;
		};
	}

	template <typename... Ts>
	struct tuple
		:
		detail::tuple_base<detail::type_pack<Ts...>, std::index_sequence_for<Ts...>>
	{ };

	template <typename... Ts>
	tuple(Ts const&...)->tuple<Ts...>;

	// -- Tuple protocol
	template <typename ... Ts>
	struct tup_size<tuple<Ts...>>
		: std::integral_constant<std::size_t, sizeof...(Ts)> {};

	template <std::size_t I, typename ... Ts>
	struct tup_elem<I, tuple<Ts...>>
		: decltype(tuple<Ts...>::template get_type<I>()) {};
	// --

	namespace detail
	{
		template <std::size_t I, typename Tuple>
		constexpr decltype(auto) tuple_get(Tuple&& t) noexcept
		{
			using tuple_t = remove_cvref_t<Tuple>;
			static_assert(I < tup_size_v<tuple_t>, "Index out of range");
			return std::forward<Tuple>(t)[zuic<I>];
		}
	}

	template <std::size_t I, typename... Ts>
	constexpr decltype(auto) get(tuple<Ts...> const& t) noexcept
	{
		return detail::tuple_get<I>(t);
	}

	template <std::size_t I, typename... Ts>
	constexpr decltype(auto) get(tuple<Ts...>& t) noexcept
	{
		return detail::tuple_get<I>(t);
	}

	template <std::size_t I, typename... Ts>
	constexpr decltype(auto) get(tuple<Ts...>&& t) noexcept
	{
		return detail::tuple_get<I>(std::move(t));
	}
}

namespace std
{
	// Specialize
	//
	//  * std::tuple_size
	//  * std::tuple_element
	//
	// to allow structured bindings to tr::tuple

	template <typename... Ts>
	struct tuple_size<::tr::tuple<Ts...>>
		: integral_constant<size_t, sizeof...(Ts)> {};

	template <size_t I, typename... Ts>
	struct tuple_element<I, ::tr::tuple<Ts...>>
		: decltype(::tr::tuple<Ts...>::get_type(::tr::zuic<I>)) {};
}
