# 模块名
set(MODULE_NAME "spdlog")

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

function(spdlog)
    # 压缩包位置
    set(PKG_ZIP_PATH ${CMAKE_CURRENT_LIST_DIR}/../spdlog-1.14.1.zip)
    # 存放根目录
    set(MOUDLE_ROOT ${CMAKE_BINARY_DIR}/_deps)
    # 解压位置
    set(MOUDLE_SRC_PATH ${MOUDLE_ROOT}/src/${MODULE_NAME})
    # 安装位置
    set(MODULE_INSTALL_PATH ${MOUDLE_ROOT}/Install/${MODULE_NAME})
    # 编译位置
    set(MODULE_BIN_PATH ${MOUDLE_ROOT}/build/${MODULE_NAME})
    # 库目录位置
    set(MODULE_LIB_PATH ${MODULE_INSTALL_PATH}/lib)
    # 头文件位置
    set(${MODULE_NAME}_INCLUDE_PATH ${MODULE_INSTALL_PATH}/include CACHE STRING "${MODULE_NAME} include")
    # 库文件位置
    set(${MODULE_NAME}_LIBRARY_PATH ${MODULE_INSTALL_PATH}/lib CACHE STRING "${MODULE_NAME} library")
    # 编译设置
    set(MODULE_CONFIG   
    -DCMAKE_INSTALL_PREFIX=${MODULE_INSTALL_PATH}               # 设置静态库安装位置
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${CMAKE_BINARY_DIR}/bin    # 设置动态库的安装位置
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}                                  # 以Release模式编译
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    # 以下选项要去查三方库的编译选项
    -DSPDLOG_BUILD_EXAMPLE=OFF                                  # 禁止编译示例
    -DSPDLOG_BUILD_SHARED=ON)                                   # 开启编译动态库
    
    include(ExternalProject)
    set_property(DIRECTORY PROPERTY EP_BASE ${MOUDLE_ROOT})

    ExternalProject_Add(_${MODULE_NAME}
        URL     ${PKG_ZIP_PATH}
        SOURCE_DIR ${MOUDLE_SRC_PATH}
        BINARY_DIR ${MODULE_BIN_PATH}
        CMAKE_ARGS ${MODULE_CONFIG}
    )

    # 收尾工作
    add_custom_command(
        TARGET _${MODULE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${MOUDLE_ROOT}/install/_${MODULE_NAME}
        COMMENT "after build ${MODULE_NAME}"
    )
    
endfunction(spdlog)

spdlog()

link_directories(${${MODULE_NAME}_LIBRARY_PATH})
include_directories(${${MODULE_NAME}_INCLUDE_PATH})
