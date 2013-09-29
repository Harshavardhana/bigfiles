project(libbigobjects-headers C)

set(libbigobjects_HDRS
  api.h
  internal.h
  common.h
  driver.h
  uri.h
  compat.h
)

install(
  FILES
    ${libbigobjects_HDRS}
  DESTINATION
    ${INCLUDE_INSTALL_DIR}/${APPLICATION_NAME}
  COMPONENT
    headers
)
