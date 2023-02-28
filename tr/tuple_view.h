#pragma once

#include "tuple_protocol.h"

#include <cstddef>
#include <utility>

namespace tr
{
	// --
	template <std::size_t ... Is>
	struct tup_size<std::index_sequence<Is...>>
		: std::integral_constant<std::size_t, sizeof...(Is)> {};

	template <std::size_t I, std::size_t ... Is>
	constexpr std::size_t get(std::index_sequence<Is...>) noexcept
	{
		static_assert(I < sizeof...(Is), "Index out of bounds");

		std::size_t i{};
		std::size_t is{};
		((is = Is, i++ == I) || ...);
		return is;
	}
	// --

	template <typename Tuple, typename IdxPack>
	struct tuple_view;

	template <typename Tuple, std::size_t ... Is>
	struct tuple_view<Tuple, std::index_sequence<Is...>>
	{
		Tuple tuple_;
	};

	template <typename Tuple, std::size_t ... Is>
	constexpr std::index_sequence<Is...> index_seq(tuple_view<Tuple, std::index_sequence<Is...>> const&) noexcept
	{
		return {};
	};

	template <typename Tuple, std::size_t ... Is>
	struct tup_size<tuple_view<Tuple, std::index_sequence<Is...>>>
		: std::integral_constant<std::size_t, sizeof...(Is)> {};

	namespace detail
	{
		template <std::size_t I, typename TupleView>
		constexpr decltype(auto) tuple_view_get(TupleView&& tupleView) noexcept
		{
			auto is = index_seq(tupleView);
			static_assert(I < tup_size_v<decltype(is)>, "Index out of bounds");

			constexpr auto realIdx = tr::get<I>(is);
			return tr::get<realIdx>(std::forward<TupleView>(tupleView).tuple_);
		}
	}

	template <std::size_t I , typename Tuple, std::size_t ... Is>
	constexpr decltype(auto) get(tuple_view<Tuple, std::index_sequence<Is...>> const& tuple) noexcept
	{
		return detail::tuple_view_get<I>(tuple);
	}

	template <std::size_t I , typename Tuple, std::size_t ... Is>
	constexpr decltype(auto) get(tuple_view<Tuple, std::index_sequence<Is...>>& tuple) noexcept
	{
		return detail::tuple_view_get<I>(tuple);
	}

	template <std::size_t I , typename Tuple, std::size_t ... Is>
	constexpr decltype(auto) get(tuple_view<Tuple, std::index_sequence<Is...>>&& tuple) noexcept
	{
		return detail::tuple_view_get<I>(std::move(tuple));
	}

	template <typename, typename = void>
	struct tuple_view_traits;

	template <typename Tuple>
	struct tuple_view_traits<Tuple, detail::void_t<decltype(tup_size_v<Tuple>)>>
	{
		using idx_seq = std::make_index_sequence<tup_size_v<Tuple>>;
	};

	template <typename Tuple, std::size_t ... Is>
	struct tuple_view_traits<tuple_view<Tuple, std::index_sequence<Is...>>, void>
	{
		using idx_seq = std::index_sequence<Is...>;
	};

	namespace detail
	{
		template <std::size_t N, std::size_t ... Is>
		constexpr std::index_sequence<N + Is...> drop_first_idx_impl(std::index_sequence<Is...>) noexcept
		{
			return {};
		}

		template <std::size_t N, std::size_t ... Is>
		constexpr auto drop_first_idx(std::index_sequence<Is...>) noexcept
		{
			static_assert(N <= sizeof...(Is), "Dropping too many elements");
			return drop_first_idx_impl<N>(std::make_index_sequence<sizeof...(Is) - N>{});
		}
	}

	template <std::size_t N, typename Tuple>
	constexpr auto drop_first(Tuple&& tuple)
	{
		using tuple_t = detail::remove_cvref_t<Tuple>;
		using idx_seq_t = typename tuple_view_traits<tuple_t>::idx_seq;
		using res_idx_seq_t = decltype(detail::drop_first_idx<N>(idx_seq_t{}));
		return tuple_view<Tuple, res_idx_seq_t>{ std::forward<Tuple>(tuple) };
	}
}