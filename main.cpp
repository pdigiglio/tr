#include "tr/combinator.h"
#include "tr/detail/ebo.h"
#include "tr/detail/utility.h"
#include "tr/tuple.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <cassert>

// TODO:
// tr::tuple
// tr::compressed_tuple
// tr::span (with unpack capabilities)
// try and add a tag type to ebo and see if compressing capabilities increase.

namespace
{
	template <typename...>
	void foo();

	struct TestFw
	{
		void test_fw()
		{
			using tr::detail::forward_as;

			struct Base {};
			struct Derived : Base {};

			Derived d;
			static_assert(std::is_same_v<decltype(forward_as<Base, Derived&>(d)), Base&>);
			static_assert(std::is_same_v<decltype(forward_as<Base, Derived const&>(std::as_const(d))), Base const&>);

			static_assert(std::is_same_v<decltype(forward_as<Base, Derived&&>(std::move(d))), Base&&>);
			static_assert(std::is_same_v<decltype(forward_as<Base, Derived const&&>(std::move(std::as_const(d)))), Base const&&>);
		}
	};

	struct TestEbo
	{
		void value_categories()
		{
			using tr::detail::ebo;

			struct tag_t {};
			struct S {};
			ebo<S, tag_t, true> ec{};
			ebo<S, tag_t, false> e{};

			static_assert(
				std::is_same_v<decltype(get_ebo_val(ec)), decltype(get_ebo_val(e))>);

			static_assert(std::is_same_v<decltype(get_ebo_val(std::as_const(ec))),
				decltype(get_ebo_val(std::as_const(e)))>);

			static_assert(std::is_same_v<decltype(get_ebo_val(std::move(ec))),
				decltype(get_ebo_val(std::move(e)))>);

			static_assert(
				std::is_same_v<decltype(get_ebo_val(std::move(std::as_const(ec)))),
				decltype(get_ebo_val(std::move(std::as_const(e))))>);
		}

		void traits()
		{
			using tr::detail::ebo;
			using tr::detail::ebo_traits;

			struct tag_t {};
			struct S {};

			struct S_final final {};
			using func_ptr_t = void (*)();
			static_assert(ebo_traits<ebo<S, tag_t>>::is_compressed);
			static_assert(!ebo_traits<ebo<S_final, tag_t>>::is_compressed);
			static_assert(!ebo_traits<ebo<S&, tag_t>>::is_compressed);
			static_assert(!ebo_traits<ebo<int, tag_t>>::is_compressed);
			static_assert(!ebo_traits<ebo<func_ptr_t, tag_t>>::is_compressed);
		}
	};

    // Check that class tr::combinator behaves like I expect.
	struct TestCombinator
	{
		void apply()
		{
			using tr::combinator;
			constexpr std::tuple vals{ 0.1, 1, 2, 3, 4 };

			{
				constexpr combinator minComb{ [](auto i, auto j) { return i < j ? i : j; } };
				static_assert(sizeof(minComb) == 1);
				static_assert(std::is_aggregate_v<decltype(minComb)>);

				constexpr auto minVal = std::apply(
					[=](auto ... args) { return (minComb | ... | args)(); },
					vals);

				static_assert(minVal == std::get<0>(vals));
			}

			{
				constexpr combinator maxComb{ [](auto i, auto j) { return i > j ? i : j; }, 5 };
				static_assert(sizeof(maxComb) == sizeof(int));
				static_assert(std::is_aggregate_v<decltype(maxComb)>);

				constexpr auto max = std::apply(
					[=](auto ... args) { return (maxComb | ... | args)(); },
					vals);

				constexpr auto init = maxComb();
				static_assert(max == init);
			}
		}

		void value_categories()
		{
			using tr::combinator;

			{
				combinator comb{ [](int, int) {} };
				static_assert(
					std::is_rvalue_reference_v<decltype(std::move(comb).get_operator())> &&
					!std::is_const_v<std::remove_reference_t<decltype(std::move(comb).get_operator())>>);

				static_assert(
					std::is_rvalue_reference_v<decltype(std::move(std::as_const(comb)).get_operator())> &&
					std::is_const_v<std::remove_reference_t<decltype(std::move(std::as_const(comb)).get_operator())>>);

				static_assert(
					std::is_lvalue_reference_v<decltype(comb.get_operator())> &&
					!std::is_const_v<std::remove_reference_t<decltype(comb.get_operator())>>);

				static_assert(
					std::is_lvalue_reference_v<decltype(std::as_const(comb).get_operator())> &&
					std::is_const_v<std::remove_reference_t<decltype(std::as_const(comb).get_operator())>>);
			}
		}
	};

	template <typename ... Callables>
	struct overload : Callables ...
	{
		using Callables::operator()...;
	};

	template <typename ... Callables>
	overload(Callables&& ...)->overload<std::remove_reference_t<Callables>...>;
}

int main()
{
	using tr::tuple;


	constexpr std::pair p{ "hello", 1 };
	tr::for_each(p, [](auto i) { std::cout << '\'' << i << "'\n"; });

	{
		constexpr auto ok = tr::any_of(p, [](auto i) { return std::is_same_v<decltype(i), int>; });
		static_assert(ok);
	}

	{
		constexpr auto ok = tr::all_of(p, [](auto i) { return tr::detail::is_complete_v<decltype(i)>; });
		static_assert(ok);
	}

	{
		constexpr auto ok = tr::none_of(p, [](auto i) { return !tr::detail::is_complete_v<decltype(i)>; });
		static_assert(ok);
	}

	{
		static_assert(tr::count(p, 2) == 0);
		static_assert(tr::count(p, 1) == 1);

		char const  str[] = "hello";
		auto const c = tr::count_if(p,
			overload{
				[](auto) { return false; },
				[&str](char const* elem) { return std::strcmp(elem, str) == 0; }
			});

		std::printf("occurrences of '%s': %td\n", str, c);
	}

	{
		constexpr tr::tuple t{ 0,1,2,3,4,4,5 };
		constexpr auto idx = find_if_not(t, [](auto i) { return i < 5; });
		(void)t[std::integral_constant<int, idx>{}];




		//std::printf("idx %td\n", idx);
		//static_assert(idx == -1);
	}

	{
		constexpr tr::tuple t{ 0,1,2,3,4,5,5};
		constexpr auto idx = adjacent_find(t);
		//(void)t[std::integral_constant<int, idx>{}];

		//std::printf("idx %td\n", idx);
		static_assert(idx == 5);
	}



	//char const str[] = "hello";
	//tr::tuple<int, int[2], int, int> t0{ 1,{1,2},3,4 };
	//tr::tuple t1{ 2,"hallo",3,4 };





	////foo<decltype(t)>();
	////foo<decltype(b)>();
}

