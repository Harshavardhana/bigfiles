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
 */

#include <stdio.h>
#include <dlfcn.h>
#include <stdint.h>
#include <errno.h>
#include <strings.h>
#include <stdlib.h>

#include "bigobjects/driver.h"

#define SET_DEFAULT_OP(fn) do {                         \
                if (!drv->ops->fn)                      \
                        drv->ops->fn = default_##fn;    \
        } while (0)

static void
driver_defaults (driver_t *drv)
{
        if (!drv) {
                errno = -EINVAL;
                return;
        }

        SET_DEFAULT_OP (put);
        SET_DEFAULT_OP (get);
        SET_DEFAULT_OP (delete);
        /* Future OP's go here */
}

int32_t
driver_dynload (driver_t *driver)
{
        int               ret     = -1;
        char              *name   = NULL;
        void              *handle = NULL;
        class_methods_t   *cm     = NULL;
        char              *error  = NULL;


        ret = asprintf (&name, "%s/%s.so", DEFAULT_DRIVERDIR, driver->name);
        if (ret < 0)
                goto out;

        handle = dlopen (name, RTLD_NOW|RTLD_GLOBAL);
        if (!handle)
                goto out;

        driver->dlhandle = handle;

        if (!(driver->ops = dlsym (handle, "ops")))
                goto out;

        /*
         * If class_methods exists, its contents override any definitions of
         * init or fini for that driver.  Otherwise, we fall back to the
         * older method of looking for init and fini directly.
         */

        cm = dlsym (handle, "class_methods");
        if (cm) {
                driver->init        = cm->init;
                driver->fini        = cm->fini;
        }
        else {
                if (!(*VOID(&driver->init) = dlsym (handle, "init"))) {
                        error = dlerror ();
                        goto out;
                }

                if (!(*VOID(&(driver->fini)) = dlsym (handle, "fini"))) {
                        error = dlerror ();
                        goto out;
                }

        }

        driver_defaults (driver);
        ret = 0;
out:
        if (error)
                fprintf (stderr, "dlsym returned %s\n", error);

        FREE (name);
        return ret;
}


driver_t *
driver_new (struct bigobjects *bfs)
{
        driver_t  *driver = NULL;

        if (!bfs) {
                errno = -EINVAL;
                return NULL;
        }

        driver = calloc (1, sizeof(*driver));
        if (!driver) {
                errno = -ENOMEM;
                return NULL;
        }

        driver->name = bfs->driver_scheme;

        if (driver_dynload(driver) < 0)
                return NULL;

        return driver;
}


bfs_boolean_t
is_driver_valid (const char *scheme)
{
        bfs_boolean_t val = _bfs_false;

        if (!scheme) {
                errno = -EINVAL;
                return _bfs_false;
        }

        if (!strcasecmp(scheme, STORAGE_DRIVER_GLUSTER))
                val = _bfs_true;
        else if (!strcasecmp(scheme, STORAGE_DRIVER_FILE))
                val = _bfs_true;
        else
                fprintf(stderr, "Unrecognized driver type %s.\n",
                        scheme);
        return val;
}
