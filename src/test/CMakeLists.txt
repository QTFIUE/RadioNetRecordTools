cmake_minimum_required(VERSION 3.15)

set(BIN_NAME ${PROJECT_NAME}_TEST)

include_directories(../main/include)

if (NOT IS_DIRECTORY ${CMAKE_BINARY_DIR}/_dep/googletest-1.14.0)
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/_dep/
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar x ${THIRD_PARTY_LIB}/googletest-1.14.0.zip
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/_dep/
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

add_subdirectory(${CMAKE_BINARY_DIR}/_dep/googletest-1.14.0 gtest.out)

file(GLOB SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${BIN_NAME} ${SOURCE})
target_link_libraries(${BIN_NAME} gtest gmock)

enable_testing()
 
