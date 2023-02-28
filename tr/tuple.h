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
	constexpr auto&& get(tuple<Ts...> const& t)
	{
		static_assert(I < sizeof...(Ts), "Index out of range");
		return t[std::integral_constant<std::size_t, I>{}];
	}

	// -- Tuple protocol
	template <typename ... Ts>
	struct tup_size<tuple<Ts...>>
		: std::integral_constant<std::size_t, sizeof...(Ts)> {};

	template <std::size_t I, typename ... Ts>
	struct tup_elem<I, tuple<Ts...>>
		: decltype(tuple<Ts...>::template get_type<I>()) {};
	// --
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
		: decltype(::tr::tuple<Ts...>::template get_type<I>()) {};
}

namespace tr
{
	// std adaptors

	template <typename ... Ts>
	struct tup_size<::std::tuple<Ts...>>
		: ::std::tuple_size<std::tuple<Ts...>> {};

	template <std::size_t I, typename ... Ts>
	struct tup_elem<I, ::std::tuple<Ts...>>
		: ::std::tuple_element<I, ::std::tuple<Ts...>> {};

	template <typename ... Ts>
	struct tup_size<::std::pair<Ts...>>
		: ::std::tuple_size<std::pair<Ts...>> {};

	template <std::size_t I, typename ... Ts>
	struct tup_elem<I, ::std::pair<Ts...>>
		: ::std::tuple_element<I, ::std::pair<Ts...>> {};
}

namespace tr
{
	// https://www.foonathan.net/2020/05/fold-tricks/
	namespace detail
	{
		template <typename Tuple, typename UnaryFunc, std::size_t ... Is>
		constexpr UnaryFunc for_each(Tuple&& tuple, UnaryFunc pred, std::index_sequence<Is...>)
		{
			using tr::get;
			(pred(get<Is>(std::forward<Tuple>(tuple))), ...);
			return pred;
		}

		template <typename Tuple, typename Predicate, std::size_t ... Is>
		constexpr bool any_of(Tuple&& tuple, Predicate pred, std::index_sequence<Is...>)
		{
			using tr::get;
			return (pred(get<Is>(std::forward<Tuple>(tuple))) || ...);
		}

		template <typename Tuple, typename Predicate, std::size_t ... Is>
		constexpr bool all_of(Tuple&& tuple, Predicate pred, std::index_sequence<Is...>)
		{
			using tr::get;
			return (pred(get<Is>(std::forward<Tuple>(tuple))) && ...);
		}

		template <typename Tuple, typename Predicate, std::size_t ... Is>
		constexpr std::ptrdiff_t count_if(Tuple&& tuple, Predicate pred, std::index_sequence<Is...>)
		{
			using tr::get;
			return (static_cast<std::ptrdiff_t>(0) + ... + pred(get<Is>(std::forward<Tuple>(tuple))));
		}

		template <typename Tuple, typename Predicate, std::size_t ... Is>
		constexpr std::ptrdiff_t find_if(Tuple&& tuple, Predicate pred, std::index_sequence<Is...>)
		{
			using tr::get;

			bool stop{};
			std::ptrdiff_t idx{};
			((stop = pred(get<Is>(std::forward<Tuple>(tuple))), idx = Is, stop) || ...);
			return stop ? idx : -1;
			//                  ^ -1 if predicate not satisfied as it's not
			//                  easy to get the size of a tuple.

			//return idx + !stop;
			// If querying the size of a tuple becomes easier, this may be
			// considered (which is also consistend with the STL choices).
		}

		template <typename Tuple, typename BinaryPredicate, std::size_t ... Is>
		constexpr std::ptrdiff_t adjacent_find_unchecked(Tuple&& tuple, BinaryPredicate pred, std::ptrdiff_t notFound, std::index_sequence<Is...>)
		{
			bool stop{};
			std::ptrdiff_t idx{};
			((stop = pred(get<Is>(std::forward<Tuple>(tuple)), get<Is + 1>(std::forward<Tuple>(tuple))), idx = Is, stop) || ...);
			return stop ? idx : notFound;
		}

		template <typename T = void>
		struct equal_pred
		{
			T val;

			template <typename U>
			constexpr bool operator()(U&& elem) const;
		};

		template <>
		struct equal_pred<void>
		{
			template <typename T, typename U>
			constexpr bool operator()(T&& lhs, U&& rhs) const;
		};

		template <typename T>
		template <typename U>
		constexpr bool equal_pred<T>::operator()(U&& elem) const
		{
			return equal_pred<>{}(this->val, std::forward<U>(elem));
		}

		template <typename T, typename U>
		constexpr bool equal_pred<void>::operator()(T&& lhs, U&& rhs) const
		{
			auto const check = detail::check_expr([](auto&& x, auto&& y) -> decltype(x == y) {});
			if constexpr (decltype(check(lhs, rhs))::value)
			{
				return lhs == rhs;
			}
			else
			{
				return false;
			}
		}
	}

	template <typename Tuple, typename UnaryFunc>
	constexpr UnaryFunc for_each(Tuple&& tuple, UnaryFunc unaryFunc)
	{
		using tuple_t = std::remove_reference_t<Tuple>;
		auto const tupleSize = tr::tup_size_v<tuple_t>;
		detail::for_each(std::forward<Tuple>(tuple), unaryFunc, std::make_index_sequence<tupleSize>{});
		return unaryFunc;
	}

	template <typename Tuple, typename Predicate>
	constexpr bool any_of(Tuple&& tuple, Predicate pred)
	{
		using tuple_t = std::remove_reference_t<Tuple>;
		auto const tupleSize = tr::tup_size_v<tuple_t>;
		return detail::any_of(std::forward<Tuple>(tuple), pred, std::make_index_sequence<tupleSize>{});
	}

	template <typename Tuple, typename Predicate>
	constexpr bool all_of(Tuple&& tuple, Predicate pred)
	{
		using tuple_t = std::remove_reference_t<Tuple>;
		auto const tupleSize = tr::tup_size_v<tuple_t>;
		return detail::all_of(std::forward<Tuple>(tuple), pred, std::make_index_sequence<tupleSize>{});
	}

	template <typename Tuple, typename Predicate>
	constexpr bool none_of(Tuple&& tuple, Predicate pred)
	{
		return !any_of(std::forward<Tuple>(tuple), pred);
	}

	/// @brief Returns the number the elements in `tuple` that satisfies `pred`.
	/// @tparam Tuple The tuple type.
	/// @tparam Predicate The predicate type.
	/// @param tuple The tuple to examine.
	/// @param pred The unary predicate to check for.
	/// @return The number the elements in `tuple` that satisfies `pred`.
	template <typename Tuple, typename Predicate>
	constexpr std::ptrdiff_t count_if(Tuple&& tuple, Predicate pred)
	{
		using tuple_t = std::remove_reference_t<Tuple>;
		auto const tupleSize = tr::tup_size_v<tuple_t>;
		return detail::count_if(std::forward<Tuple>(tuple), pred, std::make_index_sequence<tupleSize>{});
	}

	/// @brief Returns the number the elements in `tuple` that compares equal
	/// to `val`. Note: if the expression `elem == val` is not well formed for
	/// some `elem` in `tuple`, the comparison will return `false`.
	///
	/// @tparam Tuple The tuple type.
	/// @tparam T The value type.
	/// @param tuple The tuple to examine.
	/// @param val The value to look for.
	/// @return The number the elements in `tuple` that compares equal to `val`.
	template <typename Tuple, typename T>
	constexpr std::ptrdiff_t count(Tuple&& tuple, T const& val)
	{
		return count_if(std::forward<Tuple>(tuple), detail::equal_pred<T const&>{val});
	}

	/// @brief Returns the index of the first element in `tuple` that satisfies `pred`.
	/// @tparam Tuple The tuple type.
	/// @tparam Predicate The predicate type.
	/// @param tuple The tuple to examine.
	/// @param pred The unary predicate to check for.
	/// @return The index of the first element in `tuple` that satisfies `pred`.
	template <typename Tuple, typename Predicate>
	constexpr std::ptrdiff_t find_if(Tuple&& tuple, Predicate pred)
	{
		using tuple_t = std::remove_reference_t<Tuple>;
		auto const tupleSize = tr::tup_size_v<tuple_t>;
		return detail::find_if(std::forward<Tuple>(tuple), pred, std::make_index_sequence<tupleSize>{});
	}

	/// @brief Returns the index of the first element in `tuple` that does not satisfy `pred`.
	/// @tparam Tuple The tuple type.
	/// @tparam Predicate The predicate type.
	/// @param tuple The tuple to examine.
	/// @param pred The unary predicate to check for.
	/// @return The index of the first element in `tuple` that does not satisfy `pred`.
	template <typename Tuple, typename Predicate>
	constexpr std::ptrdiff_t find_if_not(Tuple&& tuple, Predicate pred)
	{
		return find_if(std::forward<Tuple>(tuple),
			[pred](auto&& elem) { return !pred(std::forward<decltype(elem)>(elem)); });
	}

	/// @brief Returns the index of the first element in `tuple` that compares
	/// to `val`. Note: if the expression `elem == val` is not well formed for
	/// some `elem` in `tuple`, the comparison will return `false`.
	///
	/// @tparam Tuple The tuple type.
	/// @tparam T The value type.
	/// @param tuple The tuple to examine.
	/// @param val The value to look for.
	/// @return The index of the first element in `tuple` that compares equal to `val`.
	template <typename Tuple, typename T>
	constexpr std::ptrdiff_t find(Tuple&& tuple, T const& val)
	{
		return find_if(std::forward<Tuple>(tuple), detail::equal_pred<T const&>{val});
	}

	template <typename Tuple, typename BinaryPredicate>
	constexpr std::ptrdiff_t adjacent_find(Tuple&& tuple, BinaryPredicate pred)
	{
		using tuple_t = std::remove_reference_t<Tuple>;
		auto const tupleSize = tr::tup_size_v<tuple_t>;

		std::ptrdiff_t const notFound{ -1 };
		//                              ^ 
		//   -1 if predicate not satisfied as it's not easy to get the size of
		//   a tuple.

		if constexpr (tupleSize < 2)
		{
			return notFound;
		}
		else
		{
			return detail::adjacent_find_unchecked(std::forward<Tuple>(tuple), pred, notFound, std::make_index_sequence<tupleSize - 1>{});
		}
	}

	template <typename Tuple>
	constexpr std::ptrdiff_t adjacent_find(Tuple&& tuple)
	{
		return adjacent_find(std::forward<Tuple>(tuple), detail::equal_pred<>{});
	}
}
