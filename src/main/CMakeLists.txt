cmake_minimum_required(VERSION 3.15)

set(BIN_NAME ${PROJECT_NAME})

find_package(Python REQUIRED)

file(GLOB_RECURSE SRC_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

execute_process(COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/script/generator_registed_id.py 
    ${PROJECT_SOURCE_DIR}/src/main/include/store/ObjectIds.h
    ${SRC_FILES}
)

include(${PROJECT_SOURCE_DIR}/script/qt_setup.cmake) 

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS 
    ${CMAKE_CURRENT_SOURCE_DIR}/form
)

file(GLOB_RECURSE SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HEADER "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE UI "${CMAKE_CURRENT_SOURCE_DIR}/form/*.ui")
file(GLOB_RECURSE RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/resource/*.qrc")

add_executable(${BIN_NAME} ${SOURCE} ${HEADER} ${UI} ${RESOURCE})
target_link_libraries(${BIN_NAME} PUBLIC ${QT_LIBRARIES})
