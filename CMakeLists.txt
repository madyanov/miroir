# Generated using cgen 0.1.0 — https://gitlab.com/madyanov/cgen
# DO NOT EDIT

cmake_minimum_required(VERSION 3.24)
project(miroir)

#
# External packages
#

# package external/doctest
function(cgen_package_0)
    set(DOCTEST_NO_INSTALL ON CACHE INTERNAL "" FORCE)
    if(EXISTS ${PROJECT_SOURCE_DIR}/external/doctest/CMakeLists.txt)
        add_subdirectory(external/doctest)
    else()
        message(WARNING "Package external/doctest doesn't have CMakeLists.txt")
    endif()
endfunction()
if(PROJECT_IS_TOP_LEVEL)
    cgen_package_0()
endif()

# package external/yaml-cpp
function(cgen_package_1)
    set(YAML_BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
    set(YAML_CPP_BUILD_CONTRIB OFF CACHE INTERNAL "" FORCE)
    set(YAML_CPP_BUILD_TOOLS OFF CACHE INTERNAL "" FORCE)
    set(YAML_CPP_FORMAT_SOURCE OFF CACHE INTERNAL "" FORCE)
    if(EXISTS ${PROJECT_SOURCE_DIR}/external/yaml-cpp/CMakeLists.txt)
        add_subdirectory(external/yaml-cpp)
    else()
        message(WARNING "Package external/yaml-cpp doesn't have CMakeLists.txt")
    endif()
endfunction()
if(PROJECT_IS_TOP_LEVEL)
    cgen_package_1()
endif()

#
# Targets
#

# target miroir
function(cgen_target_miroir)
    add_library(miroir INTERFACE)
    add_library(miroir::miroir ALIAS miroir)
    target_sources(miroir
        INTERFACE
            include/miroir/miroir.hpp
    )
    target_include_directories(miroir
        INTERFACE
            include
    )
    set_target_properties(miroir PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
    )
    target_compile_options(miroir
        INTERFACE
            -Wall
            -Wextra
            -Wpedantic
            $<$<CONFIG:Release>:
                -Werror
            >
    )
endfunction()
cgen_target_miroir()

# target miroir_test
function(cgen_target_miroir_test)
    add_executable(miroir_test)
    target_sources(miroir_test
        PRIVATE
            tests/miroir_test.cpp
    )
    target_link_libraries(miroir_test
        PRIVATE
            doctest::doctest_with_main
            yaml-cpp
            miroir::miroir
    )
    set_target_properties(miroir_test PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
    )
    target_compile_options(miroir_test
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            $<$<CONFIG:Asan>:
                ${CMAKE_CXX_FLAGS_DEBUG}
                -O1
                -fno-omit-frame-pointer
                -fno-optimize-sibling-calls
                -fsanitize=address
            >
            $<$<CONFIG:Release>:
                -Werror
            >
            $<$<CONFIG:Ubsan>:
                ${CMAKE_CXX_FLAGS_DEBUG}
                -O1
                -fno-omit-frame-pointer
                -fno-optimize-sibling-calls
                -fsanitize=undefined
                -fno-sanitize-recover
            >
    )
    target_link_options(miroir_test
        PRIVATE
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
    )
endfunction()
if(PROJECT_IS_TOP_LEVEL)
    cgen_target_miroir_test()
endif()
