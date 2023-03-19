cmake_minimum_required(VERSION 3.24)
project(miroir)

# settings

set(TARGET_PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# library

add_library(miroir INTERFACE)
add_library(miroir::miroir ALIAS miroir)
target_sources(miroir INTERFACE include/miroir/miroir.hpp)
target_include_directories(miroir INTERFACE include)
set_target_properties(miroir PROPERTIES ${TARGET_PROPERTIES})

# tests

if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory(external)

    add_executable(miroir_test)
    target_sources(miroir_test PRIVATE tests/miroir_test.cpp)

    target_link_libraries(miroir_test
        PRIVATE doctest::doctest_with_main
        PRIVATE yaml-cpp

        PRIVATE miroir::miroir
    )

    set_target_properties(miroir_test PROPERTIES ${TARGET_PROPERTIES})

    target_compile_options(miroir_test
        PRIVATE -Wall -Wextra -Wpedantic

        PRIVATE $<$<CONFIG:Release>:-Werror>

        PRIVATE $<$<CONFIG:Asan>:
            ${CMAKE_CXX_FLAGS_DEBUG}

            -O1
            -fno-omit-frame-pointer
            -fno-optimize-sibling-calls

            -fsanitize=address
        >

        PRIVATE $<$<CONFIG:Ubsan>:
            ${CMAKE_CXX_FLAGS_DEBUG}

            -O1
            -fno-omit-frame-pointer
            -fno-optimize-sibling-calls

            -fsanitize=undefined
            -fno-sanitize-recover
        >

        PRIVATE $<$<CONFIG:Coverage>:
            ${CMAKE_CXX_FLAGS_DEBUG}
            -fprofile-arcs
            -ftest-coverage
        >
    )

    target_link_options(miroir_test
        PRIVATE $<$<CONFIG:Asan>:
            ${CMAKE_EXE_LINKER_FLAGS_DEBUG}
            -g
            -fsanitize=address
        >

        PRIVATE $<$<CONFIG:Ubsan>:
            ${CMAKE_EXE_LINKER_FLAGS_DEBUG}
            -g
            -fsanitize=undefined
        >

        PRIVATE $<$<CONFIG:Coverage>:
            ${CMAKE_EXE_LINKER_FLAGS_DEBUG}
            -fprofile-arcs
        >
    )
endif()
