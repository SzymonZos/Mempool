set(MP_LIB lib_mempool)

add_library(${MP_LIB} dummy.cpp)
target_include_directories(${MP_LIB} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
