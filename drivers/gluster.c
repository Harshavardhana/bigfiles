/*
  Author: Harshavardhana <fharshav@redhat.com>
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
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <api/glfs.h>
#include <bigobjects/api.h>

static
struct glfs *bigobject_gluster_init (bigobjects_t *bfs)
{
        glfs_t *glfs = NULL;
        int old_errno;
        int ret = 0;

        if (!bfs) {
                fprintf(stderr,"Usage: file=[filesystem][+transport]://[server[:port]]/"
                        "volname/file]");
                errno = -EINVAL;
                goto out;
        }

        glfs = glfs_new (bfs->driver_volname);

        if (!glfs)
                goto out;

        ret = glfs_set_volfile_server (glfs, "tcp",
                                       bfs->driver_server,
                                       bfs->driver_port);
        if (ret < 0)
                goto out;

        /*
         * FIXME: Use GF_LOG_ERROR instead of hard code value of 4 here when
         * GlusterFS makes GF_LOG_* macros available to libgfapi users.
         */

        ret = glfs_set_logging (glfs, "-", 4);
        if (ret < 0)
                goto out;

        ret = glfs_init (glfs);
        if (ret) {
                fprintf(stderr, "Gluster connection failed for"
                        " server=%s port=%d "
                        "volume=%s file=%s transport=tcp",
                        bfs->driver_server,
                        bfs->driver_port,
                        bfs->driver_volname,
                        bfs->driver_file);
                goto out;
        }

        return glfs;
out:
        if (glfs) {
                old_errno = errno;
                glfs_fini (glfs);
                errno = old_errno;
        }
        return NULL;
}

static int bigobject_gluster_open()
{
        return 0;
}

static int bigobject_gluster_create()
{
        return 0;
}

static int bigobject_gluster_close()
{
        return 0;
}
