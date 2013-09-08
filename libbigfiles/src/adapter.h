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

#ifndef __ADAPTER_H__
#define __ADAPTER_H__

struct _adapter;
typedef struct _adapter adapter_t;

typedef int32_t fd_t;

typedef int32_t (*fop_open_t) (adapter_t *this, int32_t flags,
                               fd_t *fd);
typedef int32_t (*fop_create_t) (adapter_t *this, int32_t flags,
                                 mode_t mode, mode_t umask, fd_t *fd);
typedef int32_t (*fop_close_t)  (adapter_t *this, fd_t *fd);

struct adapter_fops {
        fop_close_t   close;
        fop_open_t    open;
        fop_create_t  create;
};

struct _adapter {
        /* Built during parsing */
        char          *name;
        char          *type;

        /* Set after doing dlopen() */
        void                  *dlhandle;
        struct adapter_fops    *fops;
};

typedef struct _adapter adapter_t;

int adapter_dynload (adapter_t *adp);

#endif /* __ADAPTER_H__ */
