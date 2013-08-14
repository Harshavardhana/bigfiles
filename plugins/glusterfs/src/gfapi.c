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
#include <errno.h>
#include <string.h>
#include <time.h>

#include "gfapi.h"

glfs_t    *fs = NULL;

int fini ()
{
        int ret = 0;

        ret = glfs_fini (fs);

        return ret;
}

int init ()
{
        int        ret = 0;

        fs = glfs_new ("gfs");

        if (!fs) {
                fprintf (stderr, "glfs_new: returned NULL\n");
                return 1;
        }

        ret = glfs_set_volfile_server (fs, "tcp", "localhost", 24007);

        ret = glfs_set_logging (fs, "/dev/stderr", 7);

        ret = glfs_init (fs);

        return ret;
}

int
usage ()
{
        struct statvfs buf = {0,};
        int ret = 0;

        ret = glfs_statvfs (fs, "/", &buf);

        if (ret) {
                fprintf (stderr, "glfs_statvfs: error: (%s)", strerror(errno));
        }

        fprintf (stderr, "Blocksize: %lu\n", buf.f_bsize);
        return ret;
}

int
main ()
{
        int    ret = 0;

        ret = init();
        if (ret)
                fprintf (stderr, "glfs_init: returned error %d\n", ret);

        ret = usage();
        if (ret)
                fprintf (stderr, "usage(): returned error %d\n", ret);

        /* Close finally */
        ret = fini();

        return ret;
}
