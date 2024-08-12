cmake_minimum_required(VERSION 3.15)

set(BIN_NAME ${PROJECT_NAME}_TEST)

include_directories(../main/include)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/googletest-1.14.0)

file(GLOB SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${BIN_NAME} ${SOURCE})
target_link_libraries(${BIN_NAME} gtest gmock ${PROJECT_NAME}_lib)

enable_testing()
 
