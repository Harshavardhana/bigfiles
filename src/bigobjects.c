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

#include "bigobjects.h"
#include "bigobjects-private.h"
#include "uri.h"
#include "common.h"
#include "driver.h"

static bURI *
bigobject_parse_driver_uri (const char *uristr)
{
        bURI *uri = NULL;

        if (!uristr) {
                errno = -EINVAL;
                goto out;
        }

        uri = bigobject_uri_parse(uristr);
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

static struct bigobjects *
bigobject_new (const char *uristr)
{
        struct bigobjects *bfs = NULL;
        bigobject_ctx_t   *ctx = NULL;
        bURI            *uri = NULL;

        ctx = bigobject_ctx_new();
        if (!ctx)
                goto err;

        if (bigobject_ctx_defaults_init (ctx)) {
                errno = -ENOMEM;
                goto err;
        }

        bfs = calloc (1, sizeof (*bfs));
        if (!bfs) {
                errno = -ENOMEM;
                goto err;
        }

        uri = bigobject_parse_driver_uri(uristr);
        if (!uri) {
                errno = -EINVAL;
                goto err;
        }

        bfs->ctx = ctx;
        bfs->driver_scheme = strdup (uri->scheme);
        bfs->driver_port   = uri->port;
        bfs->driver_volname = strdup (uri->path);
        bfs->driver_server = strdup (uri->server);

        BF_URI_FREE(uri);

        return bfs;
err:
        return NULL;
}

int32_t
bigobject_put (struct bigobjects *bfs)
{
        int32_t ret = -1;
        return ret;
}

int32_t
bigobject_get (struct bigobjects *bfs)
{
        int32_t ret = -1;
        return ret;
}

static int32_t
bigobject_init_common (struct bigobjects *bfs)
{
        int32_t    ret = 0;
        driver_t  *driver = NULL;

        driver = driver_new(bfs);
        if (!driver) {
                ret = -1;
                goto out;
        }

        bfs->ctx->driver = driver;
out:
        if (driver)
                FREE(driver);

        return ret;
}
