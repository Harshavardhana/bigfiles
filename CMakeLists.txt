project(BigObjects)
cmake_minimum_required(VERSION 2.6)
add_subdirectory(bosh)
add_subdirectory(libbigobjects)

configure_file(libbigobjects.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libbigobjects.pc)
ADD_DEFINITIONS( -Wall -O2 -D_GNU_SOURCE )
