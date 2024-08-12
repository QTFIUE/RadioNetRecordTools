cmake_minimum_required(VERSION 3.15)

project(Test)

set(THIRD_PARTY_LIB ${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLib/cmake)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
if(MSVC)
    add_compile_options("/utf-8")
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/env/env.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/main)
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/test)
endif()

