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

#ifndef __BIGFILES_H__
#define __BIGFILES_H__

/*
  Enforce the following flags
*/

#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

#ifndef __USE_FILE_OFFSET64
#define __USE_FILE_OFFSET64
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/cdefs.h>
#include <dirent.h>
#include <sys/statvfs.h>

__BEGIN_DECLS

/* The bigfile context */
struct bfctx;
typedef struct bfctx bfctx_t;

/*
*/

int32_t bigfile_init (bfctx_t *);
int32_t bigfile_fini (bfctx_t *);

int32_t bigfile_get(void);
int32_t bigfile_put(void);

__END_DECLS

#endif /* __BIGFILES_H__ */
