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

  bigobject_get: retrieve 'object' from durable storage

  DESCRIPTION

  This function retrieves an 'object' specified by 'URI'

  PARAMETERS

  @uri: The uri string

  RETURN VALUES

   0 : Success, object retrieved
  -1 : Failure, errno set appropriately
*/

int32_t bigobject_get (const char *);

/*
  SYNOPSIS

  bigobject_put: upload an 'object' to a durable storage

  DESCRIPTION

  This function uploads an 'object' specified by 'URI'

  PARAMETERS

  @uri: The uri string

  RETURN VALUES

   0 : Success, object uploaded
  -1 : Failure, errno set appropriately
*/

int32_t bigobject_put (const char *);

/*
  SYNOPSIS

  bigobject_delete: delete an object from a durable storage

  DESCRIPTION

  This function deletes object specified by 'URI'

  PARAMETERS

  @uri: The uri string

  RETURN VALUES

   0 : Success, object deleted
  -1 : Failure, errno set appropriately
*/

int32_t bigobject_delete (const char *);

__END_DECLS

#endif /* __BIGOBJECTS_H__ */
