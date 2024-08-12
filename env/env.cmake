cmake_minimum_required(VERSION 3.15)

set(QT_PATH "D:/ProgramTools/Cpp/Qt/5.15.2/msvc2019_64" CACHE STRING "qt_path")

set(ENV_SETTING 
    "${QT_PATH}/bin"
    "${QT_PATH}/lib"
)

foreach(ITEM ${ENV_SETTING})
    message(STATUS "export ${ITEM}")
    set(ENV{PATH} "${ITEM};$ENV{PATH}")
endforeach()
