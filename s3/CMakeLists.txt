project(s3 C)

if (WITH_LIBCURL)
  set(s3_SRCS
    s3.c
    s3-iobuf.c
    s3-priv.c
    s3.h
    s3-iobuf.h
    s3-priv.h)
  include_directories(
    ${LIBCURL_INCLUDE_DIRS})
  add_library(${S3_SHARED_LIBRARY} SHARED ${s3_SRCS})
else (WITH_LIBCURL)
  message(WARNING "Cound not find libcurl - skipping building s3 driver..")
endif (WITH_LIBCURL)
