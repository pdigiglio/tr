#include <tr/algorithm/fwd/any_of.h>

#include <tr/detail/type_traits.h>
#include <tr/invoke.h>
#include <tr/unpack.h>

namespace tr {

template <typename T>
struct any_of_impl<T, std::enable_if_t<tr::is_implemented_v<unpack_impl<T>>>> {

    template <typename Tuple, typename Predicate>
    static constexpr auto apply(Tuple &&tuple, Predicate &&predicate)
        -> decltype(auto) {

        auto orElems = [&predicate](auto &&...elems) {
            return (invoke(static_cast<Predicate &&>(predicate),
                           static_cast<decltype(elems)>(elems)) ||
                    ...);
        };

        return unpack_impl<T>::apply(static_cast<Tuple &&>(tuple), orElems);
    }
};

template <typename Tuple, typename Predicate>
[[nodiscard]] constexpr auto any_of_t::operator()(Tuple &&tuple,
                                                  Predicate &&predicate) const
    -> decltype(auto) {

    using tuple_t = detail::remove_cvref_t<Tuple>;
    return any_of_impl<tuple_t>::apply(static_cast<Tuple &&>(tuple),
                                       static_cast<Predicate &&>(predicate));
}

} // namespace tr