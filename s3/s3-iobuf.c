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

#include "io-buf.h"

iobuf_t *s3_iobuf_new (void)
{
        iobuf_t *iob = NULL;

        iob = calloc (1, sizeof(*iob));
        if (!iob)
                return NULL;

        memset(iob, 0, sizeof(*iob));
        return iob;
}


int32_t s3_iobuf_add (iobuf_t *iob, char *data, int32_t len)
{
        iobufnode_t *iobnode = NULL;
        iobufnode_t *tmp_iobnode = NULL;
        int32_t     ret = -1;

        if ((!iob) || (!data))
                goto err;

        iobnode = calloc (1, sizeof(*iobnode));
        if (!iobnode)
                goto err;

        iobnode->next = NULL;
        iobnode->buf  = calloc (1, len + 1);
        memcpy (iobnode->buf, data, len);
        iobnode->buf[len] = 0;
        iob->len += len;

        if (!iob->first) {
                iob->first   = iobnode;
                iob->current = iobnode;
                iob->pos     = iobnode->buf;
        } else {
                tmp_iobnode = iob->first;
                while (tmp_iobnode->next)
                        tmp_iobnode = tmp_iobnode->next;
                tmp_iobnode->next = iobnode;
        }
        ret = len;
err:
        return ret;
}

int32_t s3_iobuf_getline (iobuf_t *iob, char *line, int32_t size)
{
        int32_t len = 0;

        memset (line, 0, size);

        if ((!iob) || (!iob->current))
                return -1;

        while ((size - len) > 1)
        {
                /* Check for newline */
                if (*iob->pos == '\n') {
                        iob->pos++;
                        line[len] = '\n';
                        len++;
                        break;
                }
                if ( *iob->pos == 0 ) {
                        iob->current = iob->current->next;
                        if (!iob->current)
                                break;
                        iob->pos = iob->current->buf;
                        continue;
                }
                line[len] = *iob->pos;
                len++;
                iob->pos++;
        }
        iob->len -= len;
        return len;
}

void s3_iobuf_free (iobuf_t *iob)
{
        iobufnode_t *iobnode = NULL;
        if (!iob)
                return;

        iobnode = iob->first;

        if (!iobnode)
                return;
        if (iob->result)
                free (iob->result);
        if (iob->lastmod)
                free (iob->lastmod);
        if (iob->etag)
                free (iob->etag);
        free (iob);

        while (iobnode->next) {
                iobnode *tmp_iobnode = iobnode->next;
                if (iobnode->buf)
                        free (iobnode->buf);
                free(iobnode);
                iobnode = tmp_iobnode;
        }

        if(iobnode)
                free(iobnode);

        return;
}
