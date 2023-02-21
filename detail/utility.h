#pragma once

#include <type_traits>
#include <utility>

namespace tr
{
	namespace detail
	{
		template<class T, class U>
		[[nodiscard]] constexpr auto&& forward_like(U&& x) noexcept
		{
			constexpr bool is_adding_const = std::is_const_v<std::remove_reference_t<T>>;
			if constexpr (std::is_lvalue_reference_v<T&&>)
			{
				if constexpr (is_adding_const)
					return std::as_const(x);
				else
					return static_cast<U&>(x);
			}
			else
			{
				if constexpr (is_adding_const)
					return std::move(std::as_const(x));
				else
					return std::move(x);
			}
		}

		template <class Base, class Derived, class U>
		[[nodiscard]] constexpr auto&& forward_as(U&& x) noexcept
		{
			static_assert(
				std::is_same_v<std::remove_reference_t<Derived>, std::remove_reference_t<U>>,
				"Derived and U must be (refrences to) the same type");

			static_assert(
				std::is_same_v<std::remove_reference_t<Base>, Base>,
				"Base must not be a reference type");

			static_assert(std::is_same_v<Base, std::remove_cv_t<Base>>,
						  "Base must not be cv qualified type");

			constexpr bool is_const = std::is_const_v<std::remove_reference_t<Derived>>;
			using cv_base_t = std::conditional_t<is_const, Base const, Base>;

			constexpr bool is_lvalue = std::is_lvalue_reference_v<Derived&&>;
			using cv_ref_base_t = std::conditional_t<is_lvalue, cv_base_t&, cv_base_t&&>;

			return static_cast<cv_ref_base_t>(x);
		}
	}
}