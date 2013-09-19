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

#ifndef _BIGFILES_PRIVATE_H
#define _BIGFILES_PRIVATE_H

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "uri.h"


struct _bigfile_ctx {
        char               *process_uuid;
};

typedef struct _bigfile_ctx bigfile_ctx_t;

struct bigfiles;
struct bigfiles {
        char             *driver_scheme;
        int               driver_port;
        char             *driver_path;
        char             *driver_server;
        bigfile_ctx_t    *ctx;
};

bigfile_ctx_t *bigfile_ctx_new ();
int bigfile_ctx_defaults_init (bigfile_ctx_t *ctx);

#endif /* !_BIGFILES_PRIVATE_H */
