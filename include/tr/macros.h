#pragma once

#if defined(_MSC_VER)

// Make sure I can exploit proper empty-base optimization on MSVC. See also:
// https://learn.microsoft.com/en-us/cpp/cpp/empty-bases
#define TR_EMPTY_BASES __declspec(empty_bases)

#else

#define TR_EMPTY_BASES /*empty*/

#endif // defined(_MSC_VER)
