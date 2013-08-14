#include <stdio.h>
#include <errno.h>
#include "api/glfs.h"
#include <string.h>
#include <time.h>

glfs_t    *fs = NULL;

void fini ()
{
        glfs_fini (fs);
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
}

int
main ()
{
        int    ret = 0;
        ret = init();

        usage();

        fini();

        fprintf (stderr, "glfs_init: returned %d\n", ret);
        return ret;
}
