#pragma once

#include <tr/algorithm/fwd/for_each.h>

#include <tr/detail/type_traits.h>
#include <tr/invoke.h>
#include <tr/unpack.h>

namespace tr {

template <typename Tuple, typename UnaryFunc>
constexpr auto for_each_t::operator()(Tuple &&tuple, UnaryFunc &&func) const
    -> UnaryFunc && {

    using tuple_t = detail::remove_cvref_t<Tuple>;
    return for_each_impl<tuple_t>::apply(static_cast<Tuple &&>(tuple),
                                         static_cast<UnaryFunc &&>(func));
}

template <typename T>
struct for_each_impl<T, std::enable_if_t<is_implemented_v<unpack_impl<T>>>> {

    template <typename Tuple, typename Func>
    static constexpr auto apply(Tuple &&tuple, Func &&func) -> Func && {

        unpack(static_cast<Tuple &&>(tuple), [&](auto &&...elems) {
            (invoke(static_cast<Func &&>(func),
                    static_cast<decltype(elems)>(elems)),
             ...);
        });

        return static_cast<Func &&>(func);
    }
};

} // namespace tr