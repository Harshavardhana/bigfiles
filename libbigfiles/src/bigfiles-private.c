#include <stdlib.h>
#include <sys/time.h>
#include <inttypes.h>
#include <unistd.h>

#include "uri.h"
#include "bigfiles-private.h"

bigfile_ctx_t *
bigfile_ctx_new ()
{
        bigfile_ctx_t     *ctx = NULL;

        ctx = calloc (1, sizeof (*ctx));
        if (!ctx) {
                errno = -ENOMEM;
                goto out;
        }
out:
        return ctx;
}

static char *
generate_bigfiles_ctx_id (void)
{
        char           tmp_str[1024] = {0,};
        char           hostname[256] = {0,};
        struct timeval tv = {0,};

        if (gettimeofday (&tv, NULL) == -1) {
                fprintf (stderr, "gettimeofday: failed %s",
                         strerror (errno));
        }

        if (gethostname (hostname, 256) == -1) {
                fprintf (stderr, "gethostname: failed %s",
                         strerror (errno));
        }

        snprintf (tmp_str, sizeof(tmp_str), "%s-%d-%"PRId64":%"PRId64,
                  hostname, getpid(), tv.tv_sec, tv.tv_usec);

        return strdup (tmp_str);
}

int
bigfile_ctx_defaults_init (bigfile_ctx_t *ctx)
{
        int  ret = -1;
        ctx->process_uuid = generate_bigfiles_ctx_id ();
        if (!ctx->process_uuid) {
                goto err;
        }
        ret = 0;
err:
        return ret;
}

bfs_boolean_t
is_valid_adapter (bURI *uri)
{
        bfs_boolean_t val = _bfs_false;

        if (!uri)
                return val;

        if (!strcmp(uri->scheme, "gluster"))
                val = _bfs_true;
        else if (!strcmp(uri->scheme, "file"))
                val = _bfs_true;
        else
                fprintf(stderr, "Unrecognized adapter type %s.\n",
                        uri->scheme);
        return val;
}
