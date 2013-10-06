project(s3 C)

if (WIN32)
  set(S3_LINK_LIBRARIES
    ${S3_LINK_LIBRARIES}
    ws2_32
  )
endif (WIN32)

if (WITH_LIBCURL)
  set(s3_SRCS
    s3.c
    s3-iobuf.c
    s3-priv.c
    s3.h
    s3-iobuf.h
    s3-priv.h)
  include_directories(${LIBCURL_INCLUDE_DIRS})
  add_library(s3 SHARED ${s3_SRCS})
  set_target_properties(s3 PROPERTIES PREFIX "")
  target_link_libraries(s3 ${LIBCURL_LIBRARIES})
else (WITH_LIBCURL)
  message(WARNING "Cound not find libcurl - skipping building s3 driver..")
endif (WITH_LIBCURL)
