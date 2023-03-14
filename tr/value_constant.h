#pragma once

#include "detail/type_traits.h"

#include <cstddef>
#include <type_traits>

namespace tr
{
	template <auto Val>
	struct value_constant {
		using type = decltype(Val);
		static constexpr type value{ Val };

		[[nodiscard]] constexpr operator type() const noexcept
		{
			return Val;
		}

		template <typename T>
		[[nodiscard]] constexpr operator T() const noexcept
		{
			return T{ Val };
			//     ^ prevent narrowing conversions (on assignments).
		}

		template <auto OtherVal>
		[[nodiscard]] friend constexpr bool operator==(value_constant, value_constant<OtherVal>) noexcept
		{
			return std::is_same_v<decltype(Val), decltype(OtherVal)> && (Val == OtherVal);
		}

		template <auto OtherVal>
		[[nodiscard]] friend constexpr bool operator!=(value_constant lhs, value_constant<OtherVal> rhs) noexcept
		{
			return !(lhs == rhs);
		}
	};

	template <auto Val>
	static constexpr value_constant<Val> value_c{};

	//template <int Val>
	//static constexpr value_constant<Val> ic{};

	//template <std::size_t Val>
	//static constexpr value_constant<Val> zuic{};

	template <unsigned Val>
	static constexpr value_constant<Val> uic{};
}