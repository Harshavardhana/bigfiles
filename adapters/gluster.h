/*
  gfapi.h: Common exposed functions

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

#ifndef __GFAPI_H__
#define __GFAPI_H__

#ifndef _CONFIG_H
#define _CONFIG_H
#include "config.h"
#endif

#include <api/glfs.h>

typedef enum {
        OPTION_TYPE_ANY = 0,
        OPTION_TYPE_STR,
        OPTION_TYPE_INT,
        OPTION_TYPE_SIZET,
        OPTION_TYPE_PERCENT,
        OPTION_TYPE_PERCENT_OR_SIZET,
        OPTION_TYPE_BOOL,
        OPTION_TYPE_PATH,
        OPTION_TYPE_TIME,
        OPTION_TYPE_DOUBLE,
        OPTION_TYPE_MAX,
} volume_option_type_t;

#define ZR_VOLUME_MAX_NUM_KEY    4
#define ZR_OPTION_MAX_ARRAY_SIZE 64

typedef struct volume_options {
        char                    *key[ZR_VOLUME_MAX_NUM_KEY];
        /* different key, same meaning */
        volume_option_type_t    type;
        double                  min;  /* 0 means no range */
        double                  max;  /* 0 means no range */
        char                    *value[ZR_OPTION_MAX_ARRAY_SIZE];
        /* If specified, will check for one of
           the value from this array */
        char                    *default_value;
        char                    *description; /* about the key */
} volume_option_t;


#endif/* __GFAPI_H__ */
