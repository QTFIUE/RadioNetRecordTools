# Qt配置
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_VERSION_MAJOR 6)
set(QT_COMPONENTS Widgets Core Gui)
set(QT_LIBRARIES "" CACHE INTERNAL "QtLibraries")

foreach(item ${QT_COMPONENTS})
    list(APPEND QT_LIBRARIES Qt${QT_VERSION_MAJOR}::${item})
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${item})
endforeach()

message(STATUS ${QT_LIBRARIES})
