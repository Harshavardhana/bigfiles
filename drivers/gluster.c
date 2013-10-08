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

#include "bigobjects/driver.h"
#include "gluster.h"

class_methods_t class_methods = {
        .init           = bobjs_gluster_init,
        .fini           = bobjs_gluster_fini
};

struct driver_ops ops = {
        .put            = bobjs_gluster_put,
        .get            = bobjs_gluster_get,
        .delete         = bobjs_gluster_delete
};

int32_t
bobjs_gluster_init (driver_t *this)
{
        glfs_t *glfs = NULL;
        int32_t ret  = -1;

        if (!this) {
                errno = -EINVAL;
                goto out;
        }

        /* Bucket name is volume name for GlusterFS */
        glfs = glfs_new (this->bucket);

        if (!glfs)
                goto out;

        ret = glfs_set_volfile_server (glfs, "tcp",
                                       this->server,
                                       this->port);
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
                        this->server,
                        this->port,
                        this->bucket,
                        this->object);
                goto out;
        }

        this->private = glfs;

        return 0;
out:
        return -1;
}

void bobjs_gluster_fini (driver_t *this)
{
        glfs_t *glfs = NULL;

        if (!this) {
                errno = -EINVAL;
                goto out;
        }

        glfs = this->private;

        if (glfs)
                glfs_fini (glfs);
out:
        return;
}

int32_t bobjs_gluster_put (driver_t *this)
{

}
