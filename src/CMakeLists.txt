project(libbigobjects-library C)

set(LIBBIGOBJECTS_PUBLIC_INCLUDE_DIRS
  ${CMAKE_SOURCE_DIR}/include
  CACHE INTERNAL "libbigobjects public include directories"
)

set(LIBBIGOBJECTS_PRIVATE_INCLUDE_DIRS
  ${CMAKE_BINARY_DIR}
)

set(LIBBIGOBJECTS_LINK_LIBRARIES
  ${LIBBIGOBJECTS_REQUIRED_LIBRARIES}
)

if (WIN32)
  set(LIBBIGOBJECTS_LINK_LIBRARIES
    ${LIBBIGOBJECTS_LINK_LIBRARIES}
    ws2_32
  )
endif (WIN32)

set(LIBBIGOBJECTS_LINK_LIBRARIES
  ${LIBBIGOBJECTS_LINK_LIBRARIES}
  CACHE INTERNAL "libbigobjects link libraries"
)

set(LIBBIGOBJECTS_SHARED_LIBRARY
  bigobjects_shared
  CACHE INTERNAL "libbigobjects shared library"
)

if (WITH_STATIC_LIB)
  set(LIBBIGOBJECTS_STATIC_LIBRARY
    bigobjects_static
    CACHE INTERNAL "libbigobjects static library"
  )
endif (WITH_STATIC_LIB)

SET (libbigobjects_SRCS
  api.c
  internal.c
  driver.c
  uri.c
  compat.c
  driver-defaults.c)

include_directories(
  ${LIBBIGOBJECTS_PUBLIC_INCLUDE_DIRS}
  ${LIBBIGOBJECTS_PRIVATE_INCLUDE_DIRS}
)

add_library(${LIBBIGOBJECTS_SHARED_LIBRARY} SHARED ${libbigobjects_SRCS})

target_link_libraries(${LIBBIGOBJECTS_SHARED_LIBRARY} ${LIBBIGOBJECTS_LINK_LIBRARIES})

set_target_properties(
  ${LIBBIGOBJECTS_SHARED_LIBRARY}
    PROPERTIES
      VERSION
        ${LIBRARY_VERSION}
      SOVERSION
        ${LIBRARY_SOVERSION}
      OUTPUT_NAME
        bigobjects
      DEFINE_SYMBOL
        LIBBIGOBJECTS_EXPORTS
)

if (WITH_VISIBILITY_HIDDEN)
  set_target_properties(${LIBBIGOBJECTS_SHARED_LIBRARY} PROPERTIES COMPILE_FLAGS "-fvisibility=hidden")
endif (WITH_VISIBILITY_HIDDEN)

install(
  TARGETS
    ${LIBBIGOBJECTS_SHARED_LIBRARY}
  RUNTIME DESTINATION ${BIN_INSTALL_DIR}
  LIBRARY DESTINATION ${LIB_INSTALL_DIR}
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT libraries
)

if (WITH_STATIC_LIB)
  add_library(${LIBBIGOBJECTS_STATIC_LIBRARY} STATIC ${libbigobjects_SRCS})

  if (MSVC)
    set(OUTPUT_SUFFIX static)
  else (MSVC)
    set(OUTPUT_SUFFIX )
  endif (MSVC)
  set_target_properties(
    ${LIBBIGOBJECTS_STATIC_LIBRARY}
      PROPERTIES
        VERSION
          ${LIBRARY_VERSION}
        SOVERSION
          ${LIBRARY_SOVERSION}
        OUTPUT_NAME
          bigobjects
        ARCHIVE_OUTPUT_DIRECTORY
          ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_SUFFIX}
  )

  if (WIN32)
    set_target_properties(
      ${LIBBIGOBJECTS_STATIC_LIBRARY}
        PROPERTIES
          COMPILE_FLAGS
            "-DLIBBIGOBJECTS_STATIC"
    )
  endif (WIN32)

  install(
    TARGETS
      ${LIBBIGOBJECTS_STATIC_LIBRARY}
    DESTINATION
      ${LIB_INSTALL_DIR}/${OUTPUT_SUFFIX}
    COMPONENT
      libraries
  )
endif (WITH_STATIC_LIB)
