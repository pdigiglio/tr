#pragma once

namespace tr {
template <typename T, T>
struct integral_constant;

template <auto Val>
using value_constant = integral_constant<decltype(Val), Val>;
} // namespace tr