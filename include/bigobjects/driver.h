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

#ifndef __DRIVER_H__
#define __DRIVER_H__

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

struct _driver;
typedef struct _driver driver_t;

#include "bigobjects/common.h"
#include "bigobjects/internal.h"
#include "bigobjects/compat.h"

typedef int32_t (*op_put_t) (driver_t *this);
typedef int32_t (*op_get_t) (driver_t *this);
typedef int32_t (*op_delete_t)  (driver_t *this);

struct driver_ops {
        op_put_t      put;
        op_get_t      get;
        op_delete_t   delete;
};

int32_t default_put (driver_t *this);

int32_t default_get (driver_t *this);

int32_t default_delete (driver_t *this);

typedef struct {
        int32_t               (*init) (driver_t *this);
        void                  (*fini) (driver_t *this);
} class_methods_t;


struct _driver {
        char                  *name;
        char                  *bucket;
        char                  *server;
        int32_t               port;
        char                  *object;

        /* Driver specific private structures */
        void                  *private;

        /* Set after doing dlopen() */
        void                  *dlhandle;
        struct driver_ops     *ops;
        int32_t               (*init) (driver_t *this);
        void                  (*fini) (driver_t *this);
};

driver_t *driver_new (struct bigobjects *);
bfs_boolean_t is_driver_valid (const char *);
int32_t driver_dynload (driver_t *);

#endif /* __DRIVER_H__ */
