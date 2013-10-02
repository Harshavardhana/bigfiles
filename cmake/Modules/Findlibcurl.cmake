find_path(LIBCURL_ROOT_DIR
    NAMES
        include/curl
    HINTS
        ${_LIBCURL_ROOT_HINTS}
    PATHS
        ${_LIBCURL_ROOT_PATHS}
)
mark_as_advanced(LIBCURL_ROOT_DIR)

if (UNIX)
  find_package(PkgConfig REQUIRED)
  if (PKG_CONFIG_FOUND)
    pkg_check_modules(LIBCURL REQUIRED libcurl)
  endif (PKG_CONFIG_FOUND)
endif (UNIX)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBCURL DEFAULT_MSG LIBCURL_LIBRARY_DIRS LIBCURL_INCLUDE_DIRS)

if (LIBCURL_INCLUDE_DIRS AND LIBCURL_LIBRARY_DIRS)
    set(LIBCURL_FOUND TRUE)
endif (LIBCURL_INCLUDE_DIRS AND LIBCURL_LIBRARY_DIRS)

# show the LIBCURL_INCLUDE_DIRS and LIBCURL_LIBRARY_DIRS variables only in the advanced view
mark_as_advanced(LIBCURL_INCLUDE_DIRS LIBCURL_LIBRARY_DIRS)
