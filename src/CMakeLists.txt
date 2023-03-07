set(MP_LIB lib_mempool)

find_package(fmt CONFIG REQUIRED)

add_library(${MP_LIB} Mempool.cpp Config.cpp)
target_include_directories(${MP_LIB} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${MP_LIB} PUBLIC fmt::fmt)
