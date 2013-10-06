# Copyright (c) 2013      Harshavardhana <fharshav@redhat.com>
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

if (UNIX)
  find_package(PkgConfig REQUIRED)
  if (PKG_CONFIG_FOUND)
    pkg_check_modules(LIBCURL REQUIRED libcurl)
  endif (PKG_CONFIG_FOUND)
endif (UNIX)

find_path(LIBCURL_ROOT_DIR
  NAMES
     include/curl/curl.h
  HINTS
     ${_LIBCURL_ROOT_HINTS}
  PATHS
     ${_LIBCURL_ROOT_PATHS}
)
mark_as_advanced(LIBCURL_ROOT_DIR)

find_path(LIBCURL_INCLUDE_DIR
  NAMES
     curl/curl.h
  PATHS
     /usr/local/include
     /opt/local/include
     /sw/include
     /usr/lib/sfw/include
     ${LIBCURL_ROOT_DIR}/include
)

set(LIBCURL_INCLUDE_DIRS ${LIBCURL_INCLUDE_DIR})
mark_as_advanced(LIBCURL_INCLUDE_DIRS)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBCURL DEFAULT_MSG LIBCURL_LIBRARIES LIBCURL_INCLUDE_DIRS)

if (LIBCURL_INCLUDE_DIRS AND LIBCURL_LIBRARIES)
    set(LIBCURL_FOUND TRUE)
endif (LIBCURL_INCLUDE_DIRS AND LIBCURL_LIBRARIES)

# show the LIBCURL_INCLUDE_DIRS and LIBCURL_LIBRARIES variables only in the advanced view
mark_as_advanced(LIBCURL_INCLUDE_DIRS LIBCURL_LIBRARIES)
