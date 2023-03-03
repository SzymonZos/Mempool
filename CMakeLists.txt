cmake_minimum_required(VERSION 3.22)
project(mempool)

set(CMAKE_CXX_STANDARD 17)

add_executable(mempool main.cpp)

include_directories(include)

add_compile_options(
    -Wall
    -Wextra
    #    -Werror
    -Wshadow # Warn if variable overshadows parent context
    -Wnon-virtual-dtor # Warn if class with virtual func has no virtual dtor
    -Wcast-align # Warn for potential performance problem casts
    -Wunused # Warn on anything being unused
    -Wno-overloaded-virtual # Don't warn if overload a virtual function
    -Wpedantic # Warn if non-standard C++ is used
    -Wconversion # Warn on type conversions that may lose data
    -Wsign-conversion # Warn on sign conversions
    -Wdouble-promotion # Warn if float is implicit promoted to double
    -Wold-style-cast # Warn if c style cast is performed
    -save-temps
)

add_compile_options(
    "$<$<CONFIG:Release>:-O3>"
)

set(MP_SANITIZER_OPTIONS -fsanitize=undefined,address)
add_compile_options(
    "$<$<CONFIG:Debug>:${MP_SANITIZER_OPTIONS}>"
)

add_link_options(
    "$<$<CONFIG:Debug>:${MP_SANITIZER_OPTIONS}>"
)

add_subdirectory(src)
add_subdirectory(test)
