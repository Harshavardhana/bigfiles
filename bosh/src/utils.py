# Copyright (c) 2013 Red Hat, Inc <http://www.redhat.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Author: Harshavardhana <fharshav@redhat.com>
#

import ctypes
import ctypes.util
import os
import sys
import time
import types

def load_library(lname):
    lib = ctypes.util.find_library(lname)
    try:
        lib_ref = ctypes.CDLL(dll)
    except ImportError:
        print('failed to find lib%s. Please install lib%s' %
              (lname, lname))
        raise
    finally:
        return lib_ref
