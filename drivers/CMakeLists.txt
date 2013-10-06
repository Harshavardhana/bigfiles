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
  add_library(gluster SHARED ${gluster_SRCS})
  set_target_properties(gluster PROPERTIES PREFIX "")
  target_link_libraries(gluster ${GFAPI_LIBRARIES})
else (WITH_GFAPI)
  message(WARNING "Cound not find gfapi - skipping building gluster driver..")
endif (WITH_GFAPI)

add_subdirectory(s3)
