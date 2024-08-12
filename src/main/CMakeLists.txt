cmake_minimum_required(VERSION 3.15)

set(BIN_NAME ${PROJECT_NAME})

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/form)

set(QT_VERSION_MAJOR 5)
set(QT_COMPONENTS Widgets Core Gui)
set(QT_LIBRARIES)
foreach(item ${QT_COMPONENTS})
    list(APPEND QT_LIBRARIES Qt${QT_VERSION_MAJOR}::${item})
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${item})
endforeach()

message(STATUS ${QT_LIBRARIES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HEADER "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE UI ${CMAKE_CURRENT_SOURCE_DIR}/form/*.ui)
file(GLOB_RECURSE RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/resource/*.qrc)

add_executable(${BIN_NAME} ${SOURCE} ${HEADER} ${UI} ${RESOURCE})

if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    add_library(${BIN_NAME}_lib SHARED ${SOURCE} ${HEADER} ${UI} ${RESOURCE})
endif()

# Qt专属连接库的方法
foreach(LIBRARIES ${QT_LIBRARIES})
    target_link_libraries(${BIN_NAME} ${LIBRARIES})
    if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
        target_link_libraries(${BIN_NAME}_lib ${LIBRARIES})
    endif()
endforeach()

# Qt编译后环境补给
add_custom_command(TARGET ${BIN_NAME} POST_BUILD
    COMMAND ${QT_PATH}/bin/windeployqt ${CMAKE_BINARY_DIR}/bin/${BIN_NAME}.exe
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${QT_PATH}/plugins/platforms  ${CMAKE_BINARY_DIR}/bin/platforms
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
