cmake_minimum_required(VERSION 3.26)
project(Leviathan)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

option(EDITOR_MODE "Run the editor." OFF)
add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>)

add_subdirectory(Leviathan)
add_subdirectory(ThirdParty)

if(EDITOR_MODE)
    add_subdirectory(LeviathanEditor)
endif()