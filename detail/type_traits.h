#pragma once

#include <type_traits>

namespace tr
{
	namespace detail
	{
		template <typename T>
		using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

		template <typename T>
		struct type_identity
		{
			using type = T;
		};

		template <typename T>
		using type_identity_t = typename type_identity<T>::type;

		//template <typename...>
		//using void_t = void;

		//template <typename T, typename = void>
		//struct is_valid : std::false_type {};

		//template <typename T>
		//struct is_valid<T, void_t<T>> : std::true_type {};

		//struct S {
		//	// void g() { std::puts("g"); }
		//	void f() { std::puts("f"); }
		//};

		//template <typename T>
		//auto call(T t)->std::enable_if_t < is_valid<decltype(T{}.f()) > ::value, void > {
		//	t.f();
		//}

		//template <typename T>
		//auto call(T t)->std::enable_if_t < is_valid<decltype(T{}.g()) > ::value, void > {
		//	t.g();
		//}

		//int main(int argc, char const* argv[]) {
		//	call(S{});

		//	using tr::ic;
		//	tr::tuple t{ "hello", "world", 1, 2, 3 };
		//	static_assert(std::is_aggregate_v<decltype(t)>);

		//	auto const [hello, world, one, two, three] = t;

		//	// foo<decltype(t)>();
		//	// foo<decltype(hello)>();

		//	std::printf("%s %s!\n", t[ic<0>], t[ic<1>]);

		//	// return t[ic<0>] = 3;

		//	// auto w1 = w;
		//	// std::puts(w.val);

		//	// S s;
		//	// S{}.f();

		//}
	}
}
