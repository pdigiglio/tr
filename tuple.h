#pragma once

#include "./detail/ebo.h"
#include "./detail/type_pack.h"
#include "./detail/type_traits.h"

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
			T const& operator[](std::integral_constant<Int, I> ic) const&
			{
				return tup_elem::subscript_impl(*this, ic);
			}

			template <typename Int>
			T& operator[](std::integral_constant<Int, I> ic)&
			{
				return tup_elem::subscript_impl(*this, ic);
			}

			template <typename Int>
			T const&& operator[](std::integral_constant<Int, I> ic) const&&
			{
				return tup_elem::subscript_impl(std::move(*this), ic);
			}

			template <typename Int>
			T&& operator[](std::integral_constant<Int, I> ic)&&
			{
				return tup_elem::subscript_impl(std::move(*this), ic);
			}

			/// @brief An undefined member function to query the type of this
			/// tup_elem in traits like std::tuple_element.
			///
			/// @tparam J An index that't only used to match it against \c I.
			///
			/// @return `type_identity<T>`. If I return `T` directly, I can't
			/// use this technique to query the type of a `tup_elem<T[N]>`, as
			/// I can't return arrays.
			template <std::size_t J, typename = std::enable_if_t<I == J>>
			static type_identity<T> get_type() /* undefined */;

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

	template <std::size_t I, typename... Ts>
	auto&& get(tuple<Ts...> const& t)
	{
		return t[std::integral_constant<std::size_t, I>{}];
	}
}

namespace std
{
	template <typename... Ts>
	struct tuple_size<tr::tuple<Ts...>>
		: std::integral_constant<std::size_t, sizeof...(Ts)> {};

	template <std::size_t I, typename... Ts>
	struct tuple_element<I, tr::tuple<Ts...>>
	{
		using type =
			typename decltype(tr::tuple<Ts...>::template get_type<I>())::type;
	};
}
