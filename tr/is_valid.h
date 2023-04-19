#pragma once

#include "./fwd/is_valid.h"
#include <type_traits>

namespace tr {

namespace detail {

template <template <auto...> typename, typename, auto...>
struct is_valid_value_expr : std::false_type {};

template <template <auto...> typename Expr, auto... Vals>
struct is_valid_value_expr<Expr, std::void_t<Expr<Vals...>>, Vals...>
    : std::true_type {};

template <template <typename...> typename, typename, typename...>
struct is_valid_type_expr : std::false_type {};

template <template <typename...> typename Expr, typename... Ts>
struct is_valid_type_expr<Expr, std::void_t<Expr<Ts...>>, Ts...>
    : std::true_type {};

} // namespace detail

template <template <auto...> typename ValueExpr, auto... Vals>
struct is_valid_value_expr
    : detail::is_valid_value_expr<ValueExpr, void, Vals...> {};

template <template <auto...> typename ValueExpr, auto... Vals>
static constexpr bool is_valid_value_expr_v{
    is_valid_value_expr<ValueExpr, Vals...>::value};

template <template <typename...> typename TypeExpr, typename... Ts>
struct is_valid_type_expr
    : detail::is_valid_type_expr<TypeExpr, void, Ts...> {};

template <template <typename...> typename TypeExpr, typename... Ts>
static constexpr bool is_valid_type_expr_v{
    is_valid_type_expr<TypeExpr, Ts...>::value};

} // namespace tr
