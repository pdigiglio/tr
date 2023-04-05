set(SOURCE_LIST
    all_of.cpp
    drop_view.cpp
    ebo.cpp
    forward_as_base.cpp
    overloaded.cpp
    std_integer_sequence.cpp
    tuple.cpp
    type_constant.cpp
    value_constant.cpp
    value_sequence.cpp
    # --
    main.cpp)

add_executable(tests ${SOURCE_LIST})
target_link_libraries(tests PRIVATE tr) 
target_include_directories(tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")

if (MSVC)
    target_compile_options(tests PRIVATE /W4)
else()
    target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(tests PRIVATE -Wno-missing-braces)
    endif()
endif()
