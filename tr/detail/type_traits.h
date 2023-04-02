#pragma once

#include "../type_constant.h"

#include <type_traits>

namespace tr {
namespace detail {

template <typename T>
struct remove_cvref {
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

template <typename...>
using void_t = void;

template <typename T, typename = void>
struct is_valid : std::false_type {};

template <typename T>
struct is_valid<T, void_t<T>> : std::true_type {};

template <typename T>
static constexpr bool is_valid_v{is_valid<T>::value};

template <typename, typename = void>
struct is_complete : std::false_type {};

template <typename T>
struct is_complete<T, void_t<decltype(sizeof(T))>> : std::true_type {};

template <typename T>
constexpr static bool is_complete_v{is_complete<T>::value};

template <typename Callable>
struct validity_checker {
    template <typename... Args>
    constexpr auto operator()(Args&&...) const noexcept
        -> std::is_invocable<Callable, Args&&...> {
        return {};
    }
};

template <typename T>
constexpr validity_checker<T> check_expr(T) noexcept {
    return {};
}
} // namespace detail

//// If all of `type_c<Ts>...` can be assigned to each other, `Ts...` are the
//// same type.
//// Compiles on clang: doesn't compile on MSVC
// template <typename... Ts>
// struct are_same : detail::is_valid<decltype((type_c<Ts> = ...))> {};

template <typename...>
struct are_same : std::false_type {};

template <>
struct are_same<> : std::true_type {};

template <typename T, typename... Ts>
struct are_same<T, Ts...>
    : std::integral_constant<bool, (std::is_same_v<T, Ts> && ...)> {};

template <typename... Ts>
constexpr static bool are_same_v{are_same<Ts...>::value};

template <bool Condition, typename IfTrue, typename IfFalse>
using if_ = std::conditional_t<Condition, IfTrue, IfFalse>;

template <bool Condition, typename T = void>
using require_ = std::enable_if_t<Condition, T>;
} // namespace tr

// #include <cstdio>
// #include <string_view>
// #include <type_traits>
// #include <utility>
//
// template <typename>
// struct is_int;
//
// template <>
// struct is_int<int> : std::true_type {};
//
// template <typename T>
// constexpr static auto is_int_v{ is_int<T>::value };
//
//
//// ----
//// struct unimplemented;
////
//// template <std::size_t I, typename T>
//// unimplemented get(T);
//
//// template <std::size_t I>
//// int get(int) {
////     return {};
//// }
//
// template <std::size_t I>
// int get_(int, std::integral_constant<std::size_t, I>);
//
// template <typename T>
// struct tup_size;
//
// template <typename T>
// static constexpr auto tup_size_v{ tup_size<T>::value };
//
// template <std::size_t I, typename T>
// struct tup_elem;
//
// template <std::size_t I, typename T>
// using tup_elem_t = typename tup_elem<I, T>::type;
//
// namespace detail {
//
//	template <std::size_t I>
//	struct get_w {
//		template <typename T>
//		auto operator()(T&& x) const
//			-> decltype(get_(x, std::integral_constant<std::size_t,
//I>{}));
//	};
//
//	template <typename, std::size_t, typename = void>
//	struct has_get : std::false_type {};
//
//	// template <typename T, std::size_t I>
//	// struct has_get<T, I,
//	//                std::enable_if_t<!std::is_same_v<
//	//                    decltype(::get<I>(std::declval<T>())),
//unimplemented>>>
//	//     : std::true_type {};
//
//	template <typename T, std::size_t I>
//	struct has_get<T, I, std::enable_if_t<std::is_invocable_v<get_w<I>, T>>>
//		: std::true_type {};
//
//	template <typename T, std::size_t I>
//	static constexpr bool has_get_v{ has_get<T, I>::value };
//
//	template <typename, typename, typename = void>
//	struct is_tuple_like_impl : std::false_type {};
//
//	template <typename T, std::size_t... Is>
//	struct is_tuple_like_impl<
//		T, std::index_sequence<Is...>,
//		std::enable_if_t<(has_get_v<T, Is> && ...) &&
//		(is_complete_v<tup_elem_t<Is, T>> && ...)>>
//		: std::true_type {};
//
//}  // namespace detail
//
// template <typename, typename = void>
// struct is_tuple_like : std::false_type {};
//
// template <typename T>
// struct is_tuple_like<T, void_t<decltype(tup_size_v<T>)>>
//	: detail::is_tuple_like_impl<T, std::make_index_sequence<tup_size_v<T>>>
//{};
//
// template <typename T>
// static constexpr bool is_tuple_like_v{ is_tuple_like<T>::value };
//// ----
//
// template <>
// struct tup_size<int> : std::integral_constant<std::size_t, 2> {};
//
// template <std::size_t I>
// struct tup_elem<I, int> {
//	static_assert(I < tup_size_v<int>);
//	using type = int;
//};
//
// template <std::size_t I>
// int get_(int, std::integral_constant<std::size_t, I>) {
//	return {};
//}
//
// template <typename T>
// auto foo(T) -> std::enable_if_t<is_tuple_like_v<T>> {
//	std::puts("int (tuple)");
//}
//
//// template <typename T>
//// auto foo(T) -> std::enable_if_t<!is_tuple_like_v<T>> {
////     std::puts("int (non-tuple)");
//// }
//
// int main() {
//	// using t = tup_elem_t<0, int>;
//	// foo(0);
//	// foo(0.);
//
//	detail::get_w<0>{}(0);
//
//	// struct S;
//	// return is_complete<S>::value;
//	return std::is_invocable<detail::get_w<0>,
//		int>{};  // detail::has_get_v<int, 0>;
//}
