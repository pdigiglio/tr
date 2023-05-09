#pragma once

#include <tr/algorithm/fwd/fold_left.h>

#include <tr/combinator.h>
#include <tr/detail/utility.h>
#include <tr/unpack.h>

namespace tr {

template <typename T>
struct fold_left_impl<T, std::enable_if_t<is_implemented_v<unpack_impl<T>>>> {

    template <typename Tuple, typename Init, typename Combinator>
    static constexpr auto apply(Tuple &&tuple, Init &&init, Combinator &&c)
        -> decltype(auto) {

        auto fold = [&](auto &&...elems) -> decltype(auto) {
            combinator comb{static_cast<Combinator &&>(c),
                            static_cast<Init &&>(init)};

            auto foldExpr =
                (std::move(comb) | ... | static_cast<decltype(elems)>(elems));
            return detail::lref_or_value(std::move(foldExpr).value());
        };

        return unpack(static_cast<Tuple &&>(tuple), fold);
    }
};

template <typename Tuple, typename Init, typename Combinator>
[[nodiscard]] constexpr auto fold_left_t::operator()(Tuple &&tuple, Init &&init,
                                                     Combinator &&c) const
    -> decltype(auto) {

    using tuple_t = detail::remove_cvref_t<Tuple>;
    return fold_left_impl<tuple_t>::apply(static_cast<Tuple &&>(tuple),
                                          static_cast<Init &&>(init),
                                          static_cast<Combinator &&>(c));
}

} // namespace tr