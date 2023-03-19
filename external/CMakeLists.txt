# doctest
function(include_doctest)
    set(DOCTEST_NO_INSTALL ON CACHE INTERNAL "" FORCE)
    add_subdirectory(doctest)
endfunction()
include_doctest()

# yaml-cpp
function(include_yamlcpp)
    set(YAML_BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
    set(YAML_CPP_BUILD_CONTRIB OFF CACHE INTERNAL "" FORCE)
    set(YAML_CPP_BUILD_TOOLS OFF CACHE INTERNAL "" FORCE)
    set(YAML_CPP_FORMAT_SOURCE OFF CACHE INTERNAL "" FORCE)
    add_subdirectory(yaml-cpp)
endfunction()
include_yamlcpp()
