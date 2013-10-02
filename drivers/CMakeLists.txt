project(drivers C)

set(DRIVERS_PUBLIC_INCLUDE_DIRS
  ${CMAKE_SOURCE_DIR}/include
  CACHE INTERNAL "public include directories"
)

if (WITH_GFAPI)
  set(gluster_SRCS gluster.c)
  include_directories(
    ${DRIVERS_PUBLIC_INCLUDE_DIRS}
    ${GFAPI_INCLUDE_DIRS})
  add_library(${DRIVERS_SHARED_LIBRARY} gluster ${gluster_SRCS})
else (WITH_GFAPI)
  message(WARNING "Cound not find gfapi - skipping building gluster driver..")
endif (WITH_GFAPI)
