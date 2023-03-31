#include "tr/tuple_protocol/built_in_array.h"
#include "tr/tuple_protocol/std_integer_sequence.h"
#include "tr/tuple_protocol/std_pair.h"
#include "tr/tuple_protocol/std_tuple.h"

#include "tr/algorithm.h"
#include "tr/combinator.h"
#include "tr/detail/ebo.h"
#include "tr/detail/utility.h"
#include "tr/overloaded.h"
#include "tr/tuple.h"
#include "tr/at.h"
#include "tr/tuple_view.h"
#include "tr/value_sequence.h"
#include "tr/indices_for.h"

#include <algorithm>
#include <array>
#include <numeric>
#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <utility>

// TODO:
// tr::tuple
// tr::compressed_tuple
// tr::span (with unpack capabilities)
// try and add a tag type to ebo and see if compressing capabilities increase.

// TODO:
//  1. Do I need tup_elem ? Probably not
//  2. Change tr::get to another name to avoid ADL issues with std::get.
//  3. Some kind of cx iota to generate compile time indices.

namespace {
template <typename...>
void foo();

struct TestFw {
    void test_fw() {
        using tr::detail::forward_as;

        struct Base {};
        struct Derived : Base {};

        Derived d;
        static_assert(
            std::is_same_v<decltype(forward_as<Base, Derived &>(d)), Base &>);
        static_assert(std::is_same_v<decltype(forward_as<Base, Derived const &>(
                                         std::as_const(d))),
                                     Base const &>);

        static_assert(
            std::is_same_v<decltype(forward_as<Base, Derived &&>(std::move(d))),
                           Base &&>);
        static_assert(
            std::is_same_v<decltype(forward_as<Base, Derived const &&>(
                               std::move(std::as_const(d)))),
                           Base const &&>);
    }
};


// Check that class tr::combinator behaves like I expect.
struct TestCombinator {
    void apply() {
        using tr::combinator;
        constexpr std::tuple vals{0.1, 1, 2, 3, 4};

        {
            constexpr combinator minComb{
                [](auto i, auto j) { return i < j ? i : j; }};
            static_assert(sizeof(minComb) == 1);
            static_assert(std::is_aggregate_v<decltype(minComb)>);

            constexpr auto minVal = std::apply(
                [=](auto... args) { return (minComb | ... | args)(); }, vals);

            static_assert(minVal == std::get<0>(vals));
        }

        {
            constexpr combinator maxComb{
                [](auto i, auto j) { return i > j ? i : j; }, 5};
            static_assert(sizeof(maxComb) == sizeof(int));
            static_assert(std::is_aggregate_v<decltype(maxComb)>);

            constexpr auto max = std::apply(
                [=](auto... args) { return (maxComb | ... | args)(); }, vals);

            constexpr auto init = maxComb();
            static_assert(max == init);
        }
    }

    void value_categories() {
        using tr::combinator;

        {
            combinator comb{[](int, int) {}};
            static_assert(std::is_rvalue_reference_v<
                              decltype(std::move(comb).get_operator())> &&
                          !std::is_const_v<std::remove_reference_t<
                              decltype(std::move(comb).get_operator())>>);

            static_assert(
                std::is_rvalue_reference_v<
                    decltype(std::move(std::as_const(comb)).get_operator())> &&
                std::is_const_v<std::remove_reference_t<
                    decltype(std::move(std::as_const(comb)).get_operator())>>);

            static_assert(
                std::is_lvalue_reference_v<decltype(comb.get_operator())> &&
                !std::is_const_v<
                    std::remove_reference_t<decltype(comb.get_operator())>>);

            static_assert(std::is_lvalue_reference_v<
                              decltype(std::as_const(comb).get_operator())> &&
                          std::is_const_v<std::remove_reference_t<
                              decltype(std::as_const(comb).get_operator())>>);
        }
    }
};


template <std::size_t N, std::size_t... Is>
auto array_indexing() {

    std::size_t sizes[]{Is..., 1};
    std::reverse(std::begin(sizes), std::end(sizes));
    std::partial_sum(std::cbegin(sizes), std::cend(sizes), std::begin(sizes),
                     std::multiplies<>{});

    std::reverse(std::begin(sizes), std::end(sizes));

    tr::for_each(sizes, [](auto i) { std::printf("%zu ", i); });
    std::puts("");

    std::pair<std::size_t, std::size_t> out[sizeof...(Is)]{};
    std::inclusive_scan(
        std::cbegin(sizes) + 1, std::cend(sizes), std::begin(out),
        [](auto p, auto v) {
            return std::pair{p.second / v, p.second % v};
        },
        std::pair<std::size_t, std::size_t>{0, N});

    tr::for_each(out,
                 [](auto i) { std::printf("(%zu, %zu) ", i.first, i.second); });
    std::puts("");
}

// --
} // namespace

int main() {
    array_indexing<5, 2, 3>();

    using tr::tuple;

    constexpr std::pair p{"hello", 1};
    tr::for_each(p, [](auto i) { std::cout << '\'' << i << "'\n"; });

    {
        constexpr auto ok = tr::any_of(
            p, [](auto i) { return std::is_same_v<decltype(i), int>; });
        static_assert(ok);
    }

    {
        // constexpr auto ok = tr::all_of(p, [](auto i) { return
        // tr::detail::is_complete_v<decltype(i)>; });
        auto isComplete = [](auto *i) constexpr {
            using tr::detail::is_complete_v;
            return is_complete_v<decltype(*i)>;
        };

        struct incomplete;
        constexpr incomplete *vals[10]{};
        constexpr auto ok = tr::all_of(vals, isComplete);
        static_assert(!ok);
    }

    {
        constexpr auto ok = tr::none_of(
            p, [](auto i) { return !tr::detail::is_complete_v<decltype(i)>; });
        static_assert(ok);
    }

    {
        static_assert(tr::count(p, 2) == 0);
        static_assert(tr::count(p, 1) == 1);

        char const str[] = "hello";
        auto const c = tr::count_if(
            p, tr::overloaded{[](auto) { return false; },
                              [&str](char const *elem) {
                                  return std::strcmp(elem, str) == 0;
                              }});

        std::printf("occurrences of '%s': %td\n", str, c);
    }

    {
        constexpr tr::tuple t{0, 1, 2, 3, 4, 4, 5};
        constexpr auto idx = find_if_not(t, [](auto i) { return i < 5; });
        (void)t[std::integral_constant<int, idx>{}];

        // std::printf("idx %td\n", idx);
        // static_assert(idx == -1);
    }

    {
        constexpr tr::tuple t{0, 1, 2, 3, 4, 5, 5};
        constexpr auto idx = adjacent_find(t);
        //(void)t[std::integral_constant<int, idx>{}];

        // std::printf("idx %td\n", idx);
        static_assert(idx == 5);
    }

    {
        // tr::tuple t{ 0,0,0,0,0 };
        constexpr auto size = 128;
        std::uint16_t t[size]{};
        // tr::for_each(tr::iota_for(t), [&t](auto i) { tr::get(t, i) = i; });
        tr::for_each(tr::indices_for(t),
                     [&t](auto i) { tr::at(t, i) = std::uint16_t{i}; });

        // auto view0 = tr::drop_first<1>(t);
        // auto view1 = tr::drop_first<1>(view0);
        tr::for_each(tr::drop_first<10>(t), [](auto &elem) { elem = 0; });
        tr::for_each(std::as_const(t),
                     [](auto elem) { std::printf("%d ", elem); });
        std::puts("");

        // foo<decltype(view)>();
    }

    //{
    //    constexpr int arr[3][5]{{0, 1, 2}};

    //    static constexpr auto atImpl = [](auto &&arr, auto idx) -> decltype(auto) {
    //        return std::forward<decltype(arr)>(arr)[idx];
    //    };

    //    constexpr tr::combinator at{atImpl, arr};
    //    //foo<decltype(at)>();

    //    //constexpr auto at_0 = at | 0;
    //    //foo<decltype(at_0)>();

    //    //constexpr auto at_00 = at_0 | 1;
    //    //foo<decltype(at_00)>();

    //    static_assert((at | tr::value_c<0> | 1)() == 1);

    //    //return (at | ... | tr::value_c<Is>)();

    //    flat_array_at<1>(arr);
    //}

    //{
    //    int arr[3][4]{};
    //    tr::apply(tr::array_c<std::size_t, 0, 1>, []() {});
    //}

    // char const str[] = "hello";
    // tr::tuple<int, int[2], int, int> t0{ 1,{1,2},3,4 };
    // tr::tuple t1{ 2,"hallo",3,4 };

    ////foo<decltype(t)>();
    ////foo<decltype(b)>();
}

// namespace detail {
//	template <auto Val>
//	struct value_constant_base {
//		using type = decltype(Val);
//		static constexpr type value{ Val };
//	};
//
//	template <auto... Vals>
//	struct value_pack /* undefined */;
//
//	struct from_any {
//		template <typename T>
//		/* implicit */ constexpr from_any(T) noexcept /* undefined */;
//	};
//
//	template <typename = from_any>
//	struct value_tuple_tag {};
//
//	template <typename, typename>
//	struct value_tuple_base;
//
//	template <auto... Vals>
//	struct value_tuple_base<value_tuple_tag<>, value_pack<Vals...>> {
//		///// @brief Implicit cast to homogeneous tuple (if `T` is the
// common type
//		/// of
//		///// all my values).
//		// template <typename T, typename =
// std::enable_if_t<std::is_same_v<
//		//                           T, std::common_type_t<T,
// decltype(Vals)...>>>>
//		// constexpr operator value_tuple_base<value_tuple_tag<T>,
// Vals...>()
//		//     const noexcept {
//		//     return {};
//		// }
//	};
//
//	template <typename T, T... Vals>
//	struct value_tuple_base<value_tuple_tag<T>, value_pack<Vals...>> {
//		///// @brief Implicit cast to another tuple. This will cast both
// to:
//		/////  - an homogeneous tuple whose type `U` is such that
//casting from `T`
//		/// to
//		/////  `U` doesn't narrow;
//		/////  - an heterogeneous tuple;
//		/////
//		// template <typename U, U... Vs,
//		//           typename =
//		//               std::enable_if_t<std::is_same_v<U,
// std::common_type_t<U,
//		//               T>>>>
//		// constexpr operator value_tuple_base<value_tuple_tag<U>,
// Vs...>()
//		//     const noexcept {
//		//     return value_tuple_base<value_tuple_tag<U>,
//		//     static_cast<U>(Vals)...>{};
//		// }
//	};
//
//	template <std::size_t I, typename T, auto Val, auto... Vals>
//	constexpr auto at(
//		value_tuple_base<value_tuple_tag<T>, value_pack<Val, Vals...>>)
// noexcept { 		static_assert(I < 1 + sizeof...(Vals)); 		if
// constexpr (sizeof...(Vals) == 0 || I == 0) { 			return Val;
//		}
//		else {
//			// auto filtered = []<std::size_t...
// Is>(std::index_sequence<Is...>) {
//			//     return ([] {
//			//         if constexpr (I == Is + 1) {
//			//             return tuple_c<Vals>;
//			//         } else {
//			//             return tuple_c<>;
//			//         }
//			//     }() + ...);
//			// }(std::make_index_sequence<sizeof...(Vals)>{});
//			// return at<0>(filtered);
//
//			// -- or --
//			return at<I - 1>(
//				value_tuple_base<value_tuple_tag<T>,
// value_pack<Vals...>>{});
//		}
//	}
//
//	template <typename T, auto... Vals0, typename U, auto... Vals1>
//	constexpr auto operator+(
//		value_tuple_base<value_tuple_tag<T>, value_pack<Vals0...>>,
//		value_tuple_base<value_tuple_tag<U>, value_pack<Vals1...>>)
// noexcept
//		-> value_tuple_base<value_tuple_tag<std::common_type_t<T, U>>,
//		value_pack<Vals0..., Vals1...>> {
//		return {};
//	}
//
//	template <typename T, auto... Vals0, auto Val>
//	constexpr auto operator+(
//		value_tuple_base<value_tuple_tag<T>, value_pack<Vals0...>>,
//		value_constant_base<Val>) noexcept
//		-> value_tuple_base<value_tuple_tag<std::common_type_t<T,
// decltype(Val)>>, 		value_pack<Vals0 + Val...>> { 		return
// {};
//	}
//
//	template <typename T, auto... Vals0, auto Val>
//	constexpr auto operator+(
//		value_constant_base<Val> c,
//		value_tuple_base<value_tuple_tag<T>, value_pack<Vals0...>> t)
// noexcept
//		-> decltype(t + c) {
//		return {};
//	}
// }  // namespace detail
//
// template <auto Val>
// using value_constant = detail::value_constant_base<Val>;
//
// template <auto Val>
// static constexpr value_constant<Val> value_c{};
//
///// @brief A sequence of (possibly) hererogeneous values.
// template <auto... Vals>
// using value_tuple = detail::value_tuple_base<detail::value_tuple_tag<>,
//	detail::value_pack<Vals...>>;
//
// template <auto... Vals>
// static constexpr value_tuple<Vals...> tuple_c{};
//
///// @brief A sequence of homogeneous values.
// template <typename T, auto... Vals>
// using value_sequence = std::enable_if_t<
//	std::is_same_v<T, std::common_type_t<T, decltype(Vals)...>>,
//	detail::value_tuple_base<detail::value_tuple_tag<T>,
//	detail::value_pack<Vals...>>>;
//
// template <typename T, T... Vals>
// static constexpr value_sequence<T, Vals...> array_c{};
//
// using detail::at;
//
// template <typename...>
// void foo();
//
// template <typename T, auto... Vals>
// void print(detail::value_tuple_base<detail::value_tuple_tag<T>,
//	detail::value_pack<Vals...>>
//	arg) {
//	// foo<decltype(arg)>();
//
//	fmt::print("[ ");
//	(fmt::print("{} ", Vals), ...);
//	fmt::print("]\n");
// }
//
/////// @brief A sequence of homogeneous values.
//// template <typename T, T... Vals>
//// struct value_sequence : value_tuple<Vals...> {};
//
// int main() {
//	print(value_sequence<unsigned, 1>{});  // = array_c<float, 0>;
//
//	// print(value_c<1.3f> + array_c<int, 1, 1>);
//	// return at<2>(tuple_c<0, 1, 2>);
//}

// #include <fmt/core.h>
//
// #include <climits>
// #include <cstddef>
//
// template <typename...>
// using void_t = void;
//
// template <typename, typename = void>
// struct is_valid : std::false_type {};
//
// template <typename T>
// struct is_valid<T, void_t<T>> : std::true_type {};
//
// template <typename T>
// struct type_constant {
//	using type = T;
// };
//
// template <typename T>
// type_constant(T&&)->type_constant<T>;
//
// template <typename T, typename U>
// constexpr bool operator==(type_constant<T>, type_constant<U>) noexcept {
//	return std::is_same_v<T, U>;
// }
//
// template <typename T>
// static constexpr type_constant<T> type_c{};
//
// template <typename... Ts>
// static constexpr type_constant<
//	std::conditional_t<sizeof...(Ts), std::common_type_t<Ts...>, void>>
//	common_type_c{};

//// If all of `type_c<Ts>...` can be assigned to each other, `Ts...` are the
//// same type.
// template <typename... Ts>
// struct are_same : is_valid<decltype((type_c<Ts> = ...))> {};
//
// template <typename... Ts>
// constexpr static bool are_same_v{ are_same<Ts...>::value };
//
// template <bool Condition, typename IfTrue, typename IfFalse>
// using if_ = std::conditional_t<Condition, IfTrue, IfFalse>;
//
// template <bool Condition, typename T = void>
// using require_ = std::enable_if_t<Condition, T>;
//
//
//	//template <auto... Vals>
//	//struct value_pack /* undefined */;
//
//	struct from_any {
//		template <typename T>
//		/* implicit */ constexpr from_any(T) noexcept /* undefined */;
//	};
//
//	template <typename T, auto... Vals>
//	struct value_tuple_base {
//		static_assert(are_same_v<T, from_any> || are_same_v<T,
// decltype(Vals)...>);
//	};
//
//	template <auto... Vals>
//	struct value_tuple_base<from_any, Vals...> {
//		/// @brief Implicit cast to homogeneous tuple (if all of
//`Vals...` can be
//		/// converted to `T` without narrowing).
//		template <typename T>
//		constexpr operator value_tuple_base < T, T{ Vals }... > () const
// noexcept { 			return {};
//		}
//	};
//
//	template <typename T, T... Vals>
//	struct value_tuple_base<T, Vals...> {
//		/// @brief Implicit cast to another tuple. This will cast both
// to:
//		///  - an homogeneous tuple whose type `U` is such that casting
// from `T` to
//		///  `U` doesn't narrow;
//		///  - an heterogeneous tuple;
//		template <typename U, typename U_ = if_<
//			(common_type_c<T, U> == type_c<from_any>), T, U>>
//			constexpr operator value_tuple_base < U, U_{ Vals }... >
//() const noexcept { 			return {};
//		}
//	};
//
//	template <std::size_t I, typename T, auto Val, auto... Vals>
//	constexpr auto at(value_tuple_base<T, Val, Vals...>) noexcept {
//		static_assert(I < 1 + sizeof...(Vals));
//		if constexpr (sizeof...(Vals) == 0 || I == 0) {
//			return Val;
//		}
//		else {
//			// auto filtered = []<std::size_t...
// Is>(std::index_sequence<Is...>) {
//			//     return ([] {
//			//         if constexpr (I == Is + 1) {
//			//             return tuple_c<Vals>;
//			//         } else {
//			//             return tuple_c<>;
//			//         }
//			//     }() + ...);
//			// }(std::make_index_sequence<sizeof...(Vals)>{});
//			// return at<0>(filtered);
//
//			// -- or --
//			return at<I - 1>(value_tuple_base<T, Vals...>{});
//		}
//	}
//
//	template <typename T, auto... Vals0, typename U, auto... Vals1>
//	constexpr auto operator+(value_tuple_base<T, Vals0...>,
//		value_tuple_base<U, Vals1...>) noexcept
//		-> value_tuple_base<std::common_type_t<T, U>, Vals0...,
//Vals1...> { 		return {};
//	}
//
//	template <typename T, auto... Vals0, auto Val>
//	constexpr auto operator+(value_tuple_base<T, Vals0...>,
//		value_constant_base<Val>) noexcept
//		-> value_tuple_base<std::common_type_t<T, decltype(Val)>, Vals0
//+ Val...> { 		return {};
//	}
//
//	template <typename T, auto... Vals0, auto Val>
//	constexpr auto operator+(value_constant_base<Val> val,
//		value_tuple_base<T, Vals0...> tup) noexcept
//		-> decltype(tup + val) {
//		return {};
//	}
//
// template <auto Val>
// using value_constant = detail::value_constant_base<Val>;
//
// template <auto Val>
// static constexpr value_constant<Val> value_c{};
//
///// @brief A sequence of homogeneous values.
// template <typename T, auto... Vals>
// using value_sequence = detail::value_tuple_base < T, T{ Vals }... > ;
//
// template <typename T, auto... Vals>
// static constexpr value_sequence < T, T{ Vals }... > array_c{};
//
///// @brief A sequence of (possibly) hererogeneous values.
//// template <auto... Vals>
//// using value_tuple = std::conditional_t<
////     //(type_c<decltype(Vals)> == ...),
////     (std::is_same_v<decltype(Vals),
////                     std::common_type_t<decltype(Vals)...>>&&...),
////     value_sequence<std::common_type_t<decltype(Vals)...>, Vals...>,
////     detail::value_tuple_base<detail::from_any, Vals...>>;
//
// template <auto... Vals>
// using value_tuple = detail::value_tuple_base<detail::from_any, Vals...>;
//
// template <auto... Vals>
// static constexpr value_tuple<Vals...> tuple_c{};
//
// using detail::at;
//
// namespace {
//
//	template <typename...>
//	void foo();
//
//	template <typename T, auto... Vals>
//	void print(detail::value_tuple_base<T, Vals...> args) {
//		// foo<decltype(arg)>();
//
//		fmt::print("[ ");
//		(fmt::print("{} ", Vals), ...);
//		fmt::print("]\n");
//	}
//
//	void f(value_tuple<0, 1>) {}
//}  // namespace
//
// int main() {
//
//	auto seq = array_c<int, 0, 1, 2>;
//	seq = tuple_c<0, 1, 2>;
//
//	print(array_c<int, 0, 1>);
//	print(tuple_c<0, 1.2, 3>);
//}
