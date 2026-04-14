cmake_minimum_required(VERSION 3.15)

project(RadioNetRecordTools)

set(THIRD_PARTY_LIB ${CMAKE_CURRENT_SOURCE_DIR}/3rd)

option(${PROJECT_NAME}_BUILD_TEST "Build test" ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
if(MSVC)
    add_compile_options("/utf-8")
endif()

include(FetchContent)
FetchContent_Declare(
    spdlog
    URL ${THIRD_PARTY_LIB}/spdlog-1.14.1.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(EXPECTED_BUILD_TESTS OFF)

FetchContent_MakeAvailable(spdlog)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/main)

if(${PROJECT_NAME}_BUILD_TEST)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/test)
endif()
