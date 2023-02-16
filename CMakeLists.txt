cmake_minimum_required(VERSION 3.24)
project(miroir)

set(TARGET_PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Library

add_library(miroir INTERFACE include/miroir/miroir.hpp)
add_library(miroir::miroir ALIAS miroir)
target_include_directories(miroir INTERFACE include)
set_target_properties(miroir PROPERTIES ${TARGET_PROPERTIES})

# Tests

if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory(external)

    add_executable(miroir_test tests/miroir_test.cpp)

    target_link_libraries(miroir_test
        PRIVATE doctest::doctest_with_main
        PRIVATE yaml-cpp

        PRIVATE miroir::miroir
    )

    set_target_properties(miroir_test PROPERTIES ${TARGET_PROPERTIES})

    # Configs

    target_compile_options(miroir_test PRIVATE
        -Wall -Wextra -Wpedantic

        $<$<CONFIG:Release>:-Werror>

        $<$<CONFIG:Asan>:
            ${CMAKE_CXX_FLAGS_DEBUG}

            -O1
            -fno-omit-frame-pointer
            -fno-optimize-sibling-calls

            -fsanitize=address
        >

        $<$<CONFIG:Ubsan>:
            ${CMAKE_CXX_FLAGS_DEBUG}

            -O1
            -fno-omit-frame-pointer
            -fno-optimize-sibling-calls

            -fsanitize=undefined
            -fno-sanitize-recover
        >

        $<$<CONFIG:Coverage>:
            ${CMAKE_CXX_FLAGS_DEBUG}
            -fprofile-arcs
            -ftest-coverage
        >
    )

    target_link_options(miroir_test PRIVATE
        $<$<CONFIG:Asan>:
            ${CMAKE_EXE_LINKER_FLAGS_DEBUG}
            -g
            -fsanitize=address
        >

        $<$<CONFIG:Ubsan>:
            ${CMAKE_EXE_LINKER_FLAGS_DEBUG}
            -g
            -fsanitize=undefined
        >

        $<$<CONFIG:Coverage>:
            ${CMAKE_EXE_LINKER_FLAGS_DEBUG}
            -fprofile-arcs
        >
    )
endif()
