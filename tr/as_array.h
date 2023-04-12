#pragma once

#include <type_traits>

namespace tr {

/// @brief A template alias to create a temporary array. The alias will be
/// valid only when it's passed an array type.
///
/// @details The size of the array may be deduced from the initializer list or
/// explicitly specified:
/// 
/// @code
/// // Create a temporary int[3]{0,1,2}
/// as_array<T[]>{0,1,2};
/// 
/// // Create a temporary int[5]{0,1,2,0,0}
/// as_array<T[5]>{0,1,2};
/// @endcode
/// 
/// See also:  https://en.cppreference.com/w/cpp/language/array#Array_rvalues
/// 
/// @tparam T The array type. The size may be specified or deduced by the
/// compiler.
template <typename T>
using as_array = std::enable_if_t<std::is_array_v<T>, T>;

}