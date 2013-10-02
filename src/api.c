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

#include <bigobjects/api.h>

/* INTERNAL HEADERS */
#include "bigobjects/uri.h"
#include "bigobjects/common.h"
#include "bigobjects/driver.h"
#include "bigobjects/internal.h"
/********************/

static int set_bigobject_options (struct bigobjects *bobjs,
                                  char *path)
{
        char *p, *q;

        if (!path) {
                return -EINVAL;
        }

        /* volume */
        p = q = path + strspn(path, "/");
        p += strcspn(p, "/");
        if (*p == '\0')
                return -EINVAL;

        bobjs->driver_volname = strndup(q, p - q);

        /* image */
        p += strspn(p, "/");
        if (*p == '\0')
                return -EINVAL;

        bobjs->driver_file = strdup(p);
        return 0;
}

static bURI *
bigobject_parse_driver_uri (const char *uristr)
{
        bURI *uri = NULL;

        if (!uristr) {
                errno = -EINVAL;
                goto err;
        }

        uri = bigobject_uri_parse(uristr);
        if (!uri) {
                errno = -ENOMEM;
                goto err;
        }

        if (!(is_driver_valid(uri->scheme))) {
                errno = -EINVAL;
                goto err;
        }

        return uri;
err:
        if (uri)
                BF_URI_FREE(uri);
        return NULL;
}

static struct bigobjects *
bigobject_new (const char *uristr)
{
        struct bigobjects *bobjs = NULL;
        bigobject_ctx_t   *ctx = NULL;
        bURI            *uri = NULL;

        ctx = bigobject_ctx_new();
        if (!ctx)
                goto err;

        if (bigobject_ctx_defaults_init (ctx)) {
                errno = -ENOMEM;
                goto err;
        }

        bobjs = calloc (1, sizeof (*bobjs));
        if (!bobjs) {
                errno = -ENOMEM;
                goto err;
        }

        uri = bigobject_parse_driver_uri(uristr);
        if (!uri) {
                errno = -EINVAL;
                goto err;
        }

        bobjs->ctx = ctx;
        bobjs->driver_scheme = strdup (uri->scheme);
        bobjs->driver_port = uri->port;
        if (set_bigobject_options(bobjs, uri->path)) {
                errno = -EINVAL;
                goto err;
        }

        bobjs->driver_server = strdup (uri->server);
        if (uri)
                BF_URI_FREE(uri);
        return bobjs;
err:
        return NULL;
}

int32_t
bigobject_put (const struct bigobjects *bobjs)
{
        int32_t ret = -1;

        return ret;
}

int32_t
bigobject_get (const struct bigobjects *bobjs)
{
        int32_t ret = -1;
        return ret;
}
