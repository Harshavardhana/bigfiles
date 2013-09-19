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

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>

#ifndef _CONFIG_H
#define _CONFIG_H
#include "config.h"
#endif

#include "bigfiles.h"
#include "bigfiles-private.h"
#include "uri.h"
#include "driver.h"

static bURI *
bigfile_parse_driver_uri (const char *uristr)
{
        bURI *uri = NULL;

        if (!uristr) {
                errno = -EINVAL;
                goto out;
        }

        uri = bigfile_uri_parse(uristr);
        if (!uri) {
                errno = -ENOMEM;
                goto out;
        }
        if (!(is_driver_valid(uri->scheme))) {
                errno = -EINVAL;
                goto out;
        }

        return uri;
out:
        if (uri)
                BF_URI_FREE(uri);
        return NULL;
}

struct bigfiles *
bigfile_new (const char *uristr)
{
        struct bigfiles *bfs = NULL;
        bigfile_ctx_t   *ctx = NULL;
        bURI            *uri = NULL;

        ctx = bigfile_ctx_new();
        if (!ctx)
                goto err;

        if (bigfile_ctx_defaults_init (ctx)) {
                errno = -ENOMEM;
                goto err;
        }

        bfs = calloc (1, sizeof (*bfs));
        if (!bfs) {
                errno = -ENOMEM;
                goto err;
        }

        uri = bigfile_parse_driver_uri(uristr);
        if (!uri) {
                errno = -EINVAL;
                goto err;
        }

        bfs->ctx = ctx;
        bfs->driver_scheme = strdup (uri->scheme);
        bfs->driver_port   = uri->port;
        bfs->driver_path   = strdup (uri->path);
        bfs->driver_server = strdup (uri->server);

        BF_URI_FREE(uri);

        return bfs;
err:
        return NULL;
}


static int
bigfile_init_common (struct bigfiles *bfs)
{
        int  ret = -1;
        driver_t  *adp = NULL;

        adp = driver_new(bfs);
        if (!adp)
                goto out;

        ret = driver_dynload(adp);
        if (!ret)
                goto out;
out:
        return ret;
}

int
bigfile_init (struct bigfiles *bfs)
{
        int  ret = -1;
        ret = bigfile_init_common(bfs);
        return ret;
}


int
bigfile_fini (struct bigfiles *bfs)
{
        int  ret = -1;
        return ret;
}
