#pragma once

#include "detail/type_traits.h"

namespace tr
{
	template <typename ... Callables>
	struct overload : Callables ...
	{
		using Callables::operator()...;
	};

	template <typename ... Callables>
	overload(Callables&& ...) -> overload<detail::remove_cvref_t<Callables>...>;
}