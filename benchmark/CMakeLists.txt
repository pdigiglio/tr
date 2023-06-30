include("../cmake/GoogleBenchmark.cmake")

add_executable(g_benchmark benchmark_tuple.cpp)
target_compile_features(g_benchmark PUBLIC cxx_std_17)
target_link_libraries(g_benchmark benchmark::benchmark tr)
if ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
        target_link_libraries(g_benchmark Shlwapi)
endif()
