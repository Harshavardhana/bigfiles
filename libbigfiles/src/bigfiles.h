/**
 * bigfiles.h: Common bigfiles structures
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

/* The Bigfile object.*/
struct bigfiles;
typedef struct bigfiles bigfiles_t;

/*
  SYNOPSIS

  bigfile_new: Create a new API object.

  DESCRIPTION

  This is most likely the very first function you should use.
  This function will create a new bigfiles_t API object in memory.

  PARAMETERS

  @filename: URI path specified as shown below

  - Adapter type Gluster URI format - "gluster://1.2.3.4/testvol/filename"
  - Adapter type local file format  - "file://mount_directory/filename"

  RETURN VALUES

  NULL   : Out of memory condition.
  Others : Pointer to the newly created bigfiles_t API object.

*/

bigfiles_t *bigfile_new (const char *uristr);

/*
  SYNOPSIS

  bigfile_init: Initialize the 'bigfile API'

  DESCRIPTION

  This function initializes the bigfiles_t structure.

  PARAMETERS

  @bfs: The bigfile API object to be initialized.

  RETURN VALUES

   0 : Success.
  -1 : Failure.

*/

int32_t bigfile_init (bigfiles_t *);
int32_t bigfile_fini (bigfiles_t *);


/*
  SYNOPSIS

  bigfile_get: Retrieve a file from durable storage

  DESCRIPTION

  This function retrieves specified 'filename' from bigfiles_t structure

  PARAMETERS

  @bfs: The bigfile API populated with necessary arguments

  RETURN VALUES

   N : Success, number of bytes retrieved
  -1 : Failure, errno set appropriately
*/

int32_t bigfile_get (bigfiles_t *);

/*
  SYNOPSIS

  bigfile_put: Upload a file to durable storage

  DESCRIPTION

  This function uploads a specified 'filename' from bigfiles_t structure

  PARAMETERS

  @bfs: The bigfile API populated with necessary arguments

  RETURN VALUES

   N : Success, number of bytes uploaded
  -1 : Failure, errno set appropriately
*/

int32_t bigfile_put (bigfiles_t *);

__END_DECLS

#endif /* __BIGFILES_H__ */
