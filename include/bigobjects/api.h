/**
 * bigobjects.h: Common bigobjects structures
 *
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

#ifndef __BIGOBJECTS_H__
#define __BIGOBJECTS_H__

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

/* The Bigobject object.*/
struct bigobjects;
typedef struct bigobjects bigobjects_t;

/*
  SYNOPSIS

  bigobject_get: Retrieve a file from durable storage

  DESCRIPTION

  This function retrieves specified 'filename' from bigobjects_t structure

  PARAMETERS

  @bfs: The bigobject API populated with necessary arguments

  RETURN VALUES

   N : Success, number of bytes retrieved
  -1 : Failure, errno set appropriately
*/

int32_t bigobject_get (bigobjects_t *);

/*
  SYNOPSIS

  bigobject_put: Upload a file to durable storage

  DESCRIPTION

  This function uploads a specified 'filename' from bigobjects_t structure

  PARAMETERS

  @bfs: The bigobject API populated with necessary arguments

  RETURN VALUES

   N : Success, number of bytes uploaded
  -1 : Failure, errno set appropriately
*/

int32_t bigobject_put (bigobjects_t *);

__END_DECLS

#endif /* __BIGOBJECTS_H__ */
