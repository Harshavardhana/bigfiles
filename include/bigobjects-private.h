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

#ifndef _BIGOBJECTS_PRIVATE_H_
#define _BIGOBJECTS_PRIVATE_H_

#include <stdio.h>
#include <errno.h>
#include <string.h>

struct bigobjects;

#include "driver.h"

struct _bigobject_ctx {
        char               *process_uuid;
        driver_t           *driver;
};

typedef struct _bigobject_ctx bigobject_ctx_t;

struct bigobjects {
        char             *driver_scheme;
        int32_t           driver_port;
        char             *driver_volname;
        char             *driver_server;
        bigobject_ctx_t    *ctx;
};

bigobject_ctx_t *bigobject_ctx_new (void);
int32_t bigobject_ctx_defaults_init (bigobject_ctx_t *);

#endif /* _BIGOBJECTS_PRIVATE_H_ */
