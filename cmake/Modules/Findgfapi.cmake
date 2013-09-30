find_path(GFAPI_ROOT_DIR
    NAMES
        include/glusterfs
    HINTS
        ${_GFAPI_ROOT_HINTS}
    PATHS
        ${_GFAPI_ROOT_PATHS}
)
mark_as_advanced(GFAPI_ROOT_DIR)

if (UNIX)
  find_package(PkgConfig REQUIRED)
  if (PKG_CONFIG_FOUND)
    pkg_check_modules(GFAPI REQUIRED glusterfs-api)
  endif (PKG_CONFIG_FOUND)
endif (UNIX)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GFAPI DEFAULT_MSG GFAPI_LIBRARY_DIRS GFAPI_INCLUDE_DIRS)

if (GFAPI_INCLUDE_DIRS AND GFAPI_LIBRARY_DIRS)
    set(GFAPI_FOUND TRUE)
endif (GFAPI_INCLUDE_DIRS AND GFAPI_LIBRARY_DIRS)

# show the GFAPI_INCLUDE_DIRS and GFAPI_LIBRARY_DIRS variables only in the advanced view
mark_as_advanced(GFAPI_INCLUDE_DIRS GFAPI_LIBRARY_DIRS)
