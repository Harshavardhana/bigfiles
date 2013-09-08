/*
  bigfiles.h: Common bigfiles structures

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

static bURI *
bigfile_parse_adapter_uri (const char *filename)
{
        bURI *uri = NULL;

        if (!filename) {
                errno = -EINVAL;
                goto out;
        }

        uri = bigfile_uri_parse(filename);
        if (!uri) {
                errno = -ENOMEM;
                goto out;
        }
        if (!(is_valid_adapter(uri))) {
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
bigfile_new (const char *filename)
{
        struct bigfiles *bfs = NULL;
        bigfile_ctx_t   *ctx = NULL;
        bURI            *uri = NULL;

        ctx = bigfile_ctx_new();
        if (!ctx) {
                return NULL;
        }

        if (bigfile_ctx_defaults_init (ctx))
                return NULL;

        bfs = calloc (1, sizeof (*bfs));
        if (!bfs)
                return NULL;

        uri = bigfile_parse_adapter_uri(filename);
        if (!uri)
                return NULL;

        bfs->ctx = ctx;
        bfs->adapter_scheme = strdup (uri->scheme);
        bfs->adapter_port   = uri->port;
        bfs->adapter_path   = strdup (uri->path);
        bfs->adapter_server = strdup (uri->server);

        BF_URI_FREE(uri);

        return bfs;
}


int
bigfile_init (struct bigfiles *bfs)
{
        int  ret = -1;

        return ret;
}


int
bigfile_fini (struct bigfiles *bfs)
{
        int  ret = -1;
        return ret;
}
