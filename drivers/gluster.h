/*
  Copyright (c) 2013 Red Hat, Inc. <http://www.redhat.com>

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  Author: Harshavardhana <fharshav@redhat.com>
*/

#ifndef __GLUSTER_H__
#define __GLUSTER_H__

#ifndef _CONFIG_H
#define _CONFIG_H
#include "config.h"
#endif

#include <api/glfs.h>

typedef struct BigFileGlusterState {
        struct glfs *glfs;
        struct glfs_fd *fd;
} BigFileGlusterState;

typedef struct GlusterConf {
        char *server;
        int port;
        char *volname;
        char *file;
        char *transport;
} GlusterConf;

#endif/* __GLUSTER_H__ */