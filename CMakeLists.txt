cmake_minimum_required(VERSION 3.2)

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

DllImport("dmpath" "src/dmpath")

ExeImport("tools" "")

IF(PROJECT_IS_TOP_LEVEL)
    ExeImport("test" "dmpath")
ENDIF()
