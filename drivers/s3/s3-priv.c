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

#include "s3-priv.h"

char *_base64_encode(const uchar_t *input)
{
        BIO *bmem;
        BIO *b64;
        BIO *bio;
        BUF_MEM *bptr = {0,};
        char *buf = NULL;

        b64 = BIO_new (BIO_f_base64());
        bmem = BIO_new (BIO_s_mem());
        bio = BIO_push (b64, bmem);

        BIO_set_flags (bio, BIO_FLAGS_BASE64_NO_NL);
        BIO_write (bio, input, strlen((char *)input));

        if(BIO_flush(bio) <= 0)
                return NULL;

        BIO_get_mem_ptr (bio, &bptr);

        buf = calloc (1, bptr->length);
        if (!buf)
                return NULL;

        memcpy (buf, bptr->data, (bptr->length - 1));
        buf [bptr->length - 1] = 0;

        BIO_free_all (bio);
        return buf;
}

void _chomp(char *str)
{
        while(*str && *str != '\n' && *str != '\r')
                str++;
        *str = '\0';
}
