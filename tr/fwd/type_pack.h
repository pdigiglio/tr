#pragma once

namespace tr {

/// @brief An empty class whose purpose is just to carry a parameter pack
/// around.
/// @tparam ...Ts The parameter pack to carry around.
template <typename... Ts>
struct type_pack;

} // namespace tr

