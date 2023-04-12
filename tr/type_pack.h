#pragma once

#include "./fwd/type_pack.h"
#include "./value_constant.h"

#include <type_traits>

namespace tr {

template <typename... Ts>
struct type_pack {

    template <typename... Us>
    [[nodiscard]] friend constexpr auto operator==(type_pack,
                                                   type_pack<Us...>) noexcept
        -> value_constant<false> {
        return {};
    }

    [[nodiscard]] friend constexpr auto operator==(type_pack,
                                                   type_pack) noexcept
        -> value_constant<true> {
        return {};
    }

    template <typename... Us>
    [[nodiscard]] friend constexpr auto
    operator!=(type_pack lhs, type_pack<Us...> rhs) noexcept
        -> value_constant<!decltype(lhs == rhs)::value> {
        return {};
    }
};

template <typename... Ts>
static constexpr type_pack<Ts...> type_pack_c{};

} // namespace tr