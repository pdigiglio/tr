#pragma once

namespace tr {

template <template <auto...> typename, auto...>
struct is_valid_value_expr;

template <template <typename...> typename, typename...>
struct is_valid_type_expr;

} // namespace tr