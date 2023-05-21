set(SOURCE_LIST
    all_of.cpp
    drop_view.cpp
    ebo.cpp
    fold_left.cpp
    forward_as_base.cpp
    invoke.cpp
    overloaded.cpp
    reverse_view.cpp
    std_integer_sequence.cpp
    tuple.cpp
    type_constant.cpp
    value_constant.cpp
    value_sequence.cpp
    # --
    main.cpp)

add_executable(tests ${SOURCE_LIST})
target_link_libraries(tests PRIVATE tr) 
#target_include_directories(tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")

if (MSVC)
    target_compile_options(tests PRIVATE /W4)
else()
    target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(tests PRIVATE -Wno-missing-braces)
    endif()
endif()


## TODO: The following options don't seem to work (at least, on MinTTY). I
## should test them on a real linux machine:
##   --mapping_file
##   --comment_style
##

#find_program(IWYU_PATH NAMES include-what-you-use iwyu REQUIRED)
#if (IWYU_PATH)
#    set(IWYU_PATH_AND_OPTS
#        ${IWYU_PATH}
#        -Xiwyu
#        --cxx17ns
#        --mapping_file=${CMAKE_CURRENT_SOURCE_DIR}/iwyu_mappings.imp
#        --comment_style=long)
#
#    set_property(
#        TARGET tests
#        PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${IWYU_PATH_AND_OPTS})
#endif()
