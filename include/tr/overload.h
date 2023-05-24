#pragma once

#include <tr/lazy_false.h>
#include <type_traits>

namespace tr {

template <typename Signature, typename = void>
struct overload_t {
    static_assert(lazy_false<Signature>,
                  "Signature is not a function signature");
};

template <typename Signature>
struct overload_t<Signature, std::enable_if_t<std::is_function_v<Signature>>> {

    template <typename FuncSignature = Signature>
    [[nodiscard]] constexpr auto operator()(FuncSignature *func) const noexcept
        -> FuncSignature * {
        return func;
    }

    template <typename C>
    [[nodiscard]] constexpr auto
    operator()(Signature C::*memFunc) const noexcept -> Signature C::* {
        return memFunc;
    }
};

template <typename Signature>
static constexpr overload_t<Signature> overload{};

} // namespace tr