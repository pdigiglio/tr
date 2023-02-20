#pragma once

#include "./type_traits.h"
#include "./utility.h"

#include <type_traits>

namespace tr
{
	namespace detail
	{
		template <
			typename T,
			bool IsCompressed = !std::is_reference_v<T> && !std::is_final_v<T>&& std::is_empty_v<T>>
		struct ebo : T {};

		template <typename T>
		struct ebo<T, false>
		{
			T Val_;
		};

		template <typename>
		struct is_ebo : std::false_type {};

		template <typename T, bool IsCompressed>
		struct is_ebo<ebo<T, IsCompressed>> : std::true_type {};

		template <typename T>
		static constexpr bool is_ebo_v{ is_ebo<T>::value };

		template <typename>
		struct ebo_traits;

		template <typename T, bool IsCompressed>
		struct ebo_traits<ebo<T, IsCompressed>>
		{
			using type = T;
			static constexpr bool is_compressed{ IsCompressed };
		};

		template <
			typename Ebo,
			typename = std::enable_if_t<is_ebo_v<remove_cvref_t<Ebo>>>>
		constexpr decltype(auto) get_ebo_val(Ebo&& e) noexcept
		{
			using ebo_traits_t = ebo_traits<remove_cvref_t<Ebo>>;

			if constexpr (ebo_traits_t::is_compressed)
			{
				using ebo_value_t = typename ebo_traits_t::type;
				return forward_as<ebo_value_t>(std::forward<Ebo>(e));
			}
			else
			{
				return (std::forward<Ebo>(e).Val_);
				//     ^ parenthesis preserve value category in the return type.
			}
		}
	}
}