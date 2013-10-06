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

struct s3_conf;
typedef struct s3_conf s3_conf_t;

#include "s3-iobuf.h"

/*
  INIT/FINI
*/

s3_conf_t *s3_init (const char *account_id,
                    const char *awskey_id,
                    const char *awskey,
                    const char *s3host,
                    const char *bktname);
void s3_fini (s3_conf_t *s3conf);

/*
  S3 Bucket/Object I/O functions
*/

int32_t s3_get (iobuf_t *buf, s3_conf_t *s3conf,
                const char *object);
int32_t s3_put (iobuf_t *buf, s3_conf_t *s3conf,
                const char *object);
int32_t s3_delete (iobuf_t *buf, s3_conf_t *s3conf,
                   const char *object);
