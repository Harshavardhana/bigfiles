/**
 * Copyright (C) 2013 Red Hat, Inc. <http://www.redhat.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

 * http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Harshavardhana <fharshav@redhat.com>
 *
 */
#include <stdlib.h>
#include <sys/time.h>
#include <inttypes.h>
#include <unistd.h>

#include "bigobjects/uri.h"
#include "bigobjects/internal.h"

bigobject_ctx_t *
bigobject_ctx_new ()
{
        bigobject_ctx_t     *ctx = NULL;

        ctx = calloc (1, sizeof (*ctx));
        if (!ctx) {
                errno = -ENOMEM;
                ctx = NULL;
                goto out;
        }
out:
        return ctx;
}

static char *
generate_bigobject_ctx_id (void)
{
        char           tmp_str[1024] = {0,};
        char           hostname[256] = {0,};
        struct timeval tv = {0,0};

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
bigobject_ctx_defaults_init (bigobject_ctx_t *ctx)
{
        int  ret = 0;
        ctx->process_uuid = generate_bigobject_ctx_id ();
        if (!ctx->process_uuid) {
                ret = -1;
                goto err;
        }
err:
        return ret;
}
