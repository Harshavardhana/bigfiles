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

if (GFAPI_INCLUDE_DIRS AND GFAPI_LIBRARIES)
    set(GFAPI_FOUND TRUE)
endif (GFAPI_INCLUDE_DIRS AND GFAPI_LIBRARIES)

# show the GFAPI_INCLUDE_DIRS and GFAPI_LIBRARIES variables only in the advanced view
mark_as_advanced(GFAPI_INCLUDE_DIRS GFAPI_LIBRARIES)
