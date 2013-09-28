project(BigObjects C)
cmake_minimum_required(VERSION 2.6)

# global needed variables
set(APPLICATION_NAME ${PROJECT_NAME})

set(APPLICATION_VERSION_MAJOR "1")
set(APPLICATION_VERSION_MINOR "0")
set(APPLICATION_VERSION_PATCH "0")

set(APPLICATION_VERSION "${APPLICATION_VERSION_MAJOR}.${APPLICATION_VERSION_MINOR}.${APPLICATION_VERSION_PATCH}")

# SOVERSION scheme: CURRENT.AGE.REVISION
#   If there was an incompatible interface change:
#     Increment CURRENT. Set AGE and REVISION to 0
#   If there was a compatible interface change:
#     Increment AGE. Set REVISION to 0
#   If the source code was changed, but there were no interface changes:
#     Increment REVISION.
set(LIBRARY_VERSION "0.1.0")
set(LIBRARY_SOVERSION "0")

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH
  ${CMAKE_SOURCE_DIR}/cmake/Modules
)

# add definitions
include(DefineCMakeDefaults)
include(DefinePlatformDefaults)
include(DefineCompilerFlags)
include(DefineInstallationPaths)
include(DefineOptions.cmake)

# disallow in-source build
include(MacroEnsureOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build. Please create a separate build directory and run 'cmake /path/to/${PROJECT_NAME} [options]' there.")

# add macros
include(MacroAddPlugin)
include(MacroCopyFile)

if (WITH_GFAPI)
  find_package(glusterfs-api 5 REQUIRED)
  if (NOT GFAPI_FOUND)
    message(FATAL_ERROR "Could not find GlusterFS API")
  endif (NOT GFAPI_FOUND)
endif(WITH_GFAPI)

# Find out if we have threading available
set(CMAKE_THREAD_PREFER_PTHREADS ON)
find_package(Threads)

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_subdirectory(bosh)
add_subdirectory(libbigobjects)
add_subdirectory(drivers)

# pkg-config file
configure_file(libbigobjects.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libbigobjects.pc)
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/libbigobjects.pc
  DESTINATION
    ${LIB_INSTALL_DIR}/pkgconfig
  COMPONENT
    pkgconfig
)

# cmake config files
configure_file(libbigobjects-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/libbigobjects-config.cmake @ONLY)
configure_file(libbigobjects-config-version.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/libbigobjects-config-version.cmake @ONLY)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/libbigobjects-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/libbigobjects-config-version.cmake
    DESTINATION
        ${CMAKE_INSTALL_DIR}
    COMPONENT
        devel
)

# in tree build settings
configure_file(libbigobjects-build-tree-settings.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/libbigobjects-build-tree-settings.cmake @ONLY)

if (WITH_TESTING)
  add_subdirectory(tests)
endif (WITH_TESTING)

message(STATUS "********************************************")
message(STATUS "********** ${PROJECT_NAME} build options : **********")
message(STATUS "GlusterFS API support: ${WITH_GFAPI}")
message(STATUS "Amazon S3 API support: ${WITH_S3API}")
if (WITH_INTERNAL_DOC)
    message(STATUS "Internal documentation generation")
else (WITH_INTERNAL_DOC)
    message(STATUS "Public API documentation generation")
endif (WITH_INTERNAL_DOC)
message(STATUS "********************************************")
