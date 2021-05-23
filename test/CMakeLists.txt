cmake_minimum_required(VERSION 3.14)
add_executable(unittests
    main.cpp
)

set_property(TARGET unittests PROPERTY CXX_STANDARD 14)
set_property(TARGET unittests PROPERTY CXX_STANDARD_REQUIRED TRUE)
set_property(TARGET unittests PROPERTY CXX_EXTENSIONS OFF)

if(MSVC)
    target_compile_options(unittests PRIVATE
        /W4
        /we4289 # nonstandard extension used: 'variable': loop control variable declared in the for-loop is used outside
                # the for-loop scope
        /w14555 # expression has no effect; expected expression with side- effect
        /w14905 # wide string literal cast to 'LPSTR'
        /w14906 # string literal cast to 'LPWSTR'
        /w14928 # illegal copy-initialization; more than one user-defined conversion has been implicitly applied
        /permissive- # standards conformance mode for MSVC compiler.
    )
else()
    target_compile_options(unittests PRIVATE
        -Wall
        -Wextra
        -pedantic
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wnull-dereference
        -Wformat=2
    )
endif()

# GoogleTest
include(FetchContent)
FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/master.zip)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

target_link_libraries(unittests
    PRIVATE enum-traits
    PRIVATE gmock
    PRIVATE gtest
)

add_test(NAME unit-tests COMMAND unittests)
