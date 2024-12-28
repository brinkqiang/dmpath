cmake_minimum_required(VERSION 3.21)

PROJECT(dmpath)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()

SET(DMPATH_VERSION_MAJOR "1")
SET(DMPATH_VERSION_MINOR "0")
SET(DMPATH_VERSION_PATCH "1")
SET(DMPATH_VERSION "${DMPATH_VERSION_MAJOR}.${DMPATH_VERSION_MINOR}.${DMPATH_VERSION_PATCH}")

MESSAGE(STATUS "VERSION: ${DMPATH_VERSION}")

OPTION(USE_DMPATH "use dmpath" OFF)

LibImport("libdmpath" "src/dmpath")
ModuleImport("dmflags" "thirdparty/dmflags")
ExeImport("tools" "libdmpath;dmflags")

IF(WIN32)
ADD_CUSTOM_COMMAND(
TARGET dmpath
PRE_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/task/install.bat $<TARGET_FILE_DIR:dmpath>
COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/task/uninstall.bat $<TARGET_FILE_DIR:dmpath>
COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE $<TARGET_FILE_DIR:dmpath>
)
ENDIF(WIN32)

install(DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/
        DESTINATION bin
        FILES_MATCHING PATTERN "*"
        PATTERN "debug" EXCLUDE
        PATTERN "*.pdb" EXCLUDE
        PATTERN "*.lib" EXCLUDE
        PATTERN "*.zip" EXCLUDE
        PATTERN "dmtime*" EXCLUDE
        PATTERN "dmflag*" EXCLUDE
        )


# 配置 CPack 打包选项
set(CPACK_GENERATOR "ZIP;NSIS64") # 选择打包格式，这里以 ZIP 为例
set(CPACK_PACKAGE_NAME "dmpath") # 设置包的名字
set(CPACK_PACKAGE_VENDOR "dmsoft") # 设置包的作者
set(CPACK_PACKAGE_DESCRIPTION "dmsoft dmpath") # 设置包的描述
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "dmsoft dmpath summary")
set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/icon/dmpath.ico") # 设置包的图标
set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/icon/dmpath.ico")

set(CPACK_PACKAGE_CONTACT "https://github.com/brinkqiang/dmpath")
set(CPACK_PACKAGE_VERSION "1.0.1")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")

IF(WIN32)
    SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
    ExecWait '\\\"$INSTDIR\\\\bin\\\\RelWithDebInfo\\\\install.bat\\\"'
    ")

    SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
    ExecWait '\\\"$INSTDIR\\\\bin\\\\RelWithDebInfo\\\\uninstall.bat\\\"'
    ")
ENDIF(WIN32)

set(CPACK_CMD "cpack -V -G NSIS64")

include(CPack)
