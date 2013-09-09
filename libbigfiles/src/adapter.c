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

#include "adapter.h"

adapter_t *
adapter_new (struct bigfiles *bfs)
{
        adapter_t  *adp = NULL;

        if (!bfs)
                return NULL;

        adp = calloc (1, sizeof(*adp));
        if (!adp) {
                errno = -ENOMEM;
                return NULL;
        }

        adp->type = bfs->adapter_scheme;
        return adp;
}

int
adapter_dynload (adapter_t *adp)
{
        int                ret = 0;
        char              *name = NULL;
        void              *handle = NULL;

        ret = asprintf (&name, "%s/%s.so", ADAPTERDIR, adp->type);
        if (ret < 0)
                goto out;

        handle = dlopen (name, RTLD_NOW|RTLD_GLOBAL);
        if (!handle) {
                ret = -1;
                goto out;
        }

        adp->dlhandle = handle;
        if (!(adp->fops = dlsym (handle, "fops"))) {
                ret = -1;
                goto out;
        }
out:
        if (name)
                free (name);
        return ret;
}
