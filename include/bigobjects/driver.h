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

typedef int32_t fd_t;

typedef int32_t (*fop_open_t) (driver_t *this, int32_t flags,
                               fd_t *fd);
typedef int32_t (*fop_create_t) (driver_t *this, int32_t flags,
                                 mode_t mode, mode_t umask, fd_t *fd);
typedef int32_t (*fop_close_t)  (driver_t *this, fd_t *fd);

struct driver_fops {
        fop_close_t   close;
        fop_open_t    open;
        fop_create_t  create;
};

struct _driver {
        char                  *type;
        /* Set after doing dlopen() */
        void                  *dlhandle;
        struct driver_fops   *fops;
};

typedef enum {
        DRIVER_GLUSTER,
        DRIVER_FILE,
} driver_type_t;

driver_t *driver_new (struct bigobjects *);
bfs_boolean_t is_driver_valid (const char *);

#endif /* __DRIVER_H__ */
