set(MP_TEST_SOURCES
    Mempool.cpp
    Config.cpp
    )

set(MP_TESTS mempool_tests)
add_executable(${MP_TESTS} ${MP_TEST_SOURCES})

target_include_directories(${MP_TESTS} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${MP_TESTS} PRIVATE lib_mempool gtest gtest_main pthread)
