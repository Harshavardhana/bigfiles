/*
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

#include <stdio.h>
#include <stdlib.h>

struct _iobufnode;
typedef struct _iobufnode iobufnode_t;

struct _iobufnode
{
        char *buf;
        struct _iobufnode *next;
};


struct _iobuf;
typedef struct _iobuf iobuf_t;

struct iobuf
{
        iobufnode_t *first;
        iobufnode_t *current;
        char        *pos;

        char        *reply;
        char        *lastmod;
        char        *etag;
        int32_t     contentlen;
        int32_t     len;
        int32_t     code;
};

/*
  Common functions for internal buffer allocator's
 */

iobuf_t *s3_iobuf_new (void);
in32_t s3_iobuf_add (iobuf_t *iob, char *data, int32_t len);
int32_t s3_iobuf_getline (iobuf_t *iob, char *line, int32_t size);
void s3_iobuf_free (iobuf_t *iob);
