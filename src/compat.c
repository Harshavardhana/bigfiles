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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#include "config.h"

#ifndef HAVE_GNU_EXT
#include "bigobjects/compat.h"

char *strndup (const char *s, size_t size)
{
        char *r;
        char *end = memchr(s, 0, size);

        if (end)
                /* Length + 1 */
                size = end - s + 1;

        r = malloc(size);

        if (size)
        {
                memcpy(r, s, size-1);
                r[size-1] = '\0';
        }
        return r;
}

int strcasecmp(const char *s1, const char *s2)
{
        unsigned i;

        for (i = 0; s1[i] && s2[i]; i++)
        {
                unsigned char c1 = tolower( (unsigned char) s1[i]);
                unsigned char c2 = tolower( (unsigned char) s2[i]);

                if (c1 < c2)
                        return -1;
                else if (c1 > c2)
                        return 1;
        }

        return !s2[i] - !s1[i];
}

char *strchrnul(const char *s, int c)
{
        const char *p = s;

        while (*p && (*p != c))
                p++;

        return (char *) p;
}

void *mempcpy (void *to, const void *from, size_t size)
{
        memcpy(to, from, size);
        return (char *) to + size;
}

int vasprintf(char **ret, const char *format, va_list ap)
{
        va_list ap2;
        int len= 100;        /* First guess at the size */
        if ((*ret= (char *)malloc(len)) == NULL)
                return -1;
        while (1)
        {
                int nchar;
                va_copy(ap2, ap);
                nchar = vsnprintf(*ret, len, format, ap2);
                if (nchar > -1 && nchar < len)
                        return nchar;
                if (nchar > len)
                        len= nchar+1;
                else
                        len*= 2;
                if ((*ret= (char *)realloc(*ret, len)) == NULL) {
                        free(*ret);
                        return -1;
                }
        }
}

int asprintf(char **ret, const char *format, ...)
{
        va_list ap;
        int nc;
        va_start (ap, format);
        nc = vasprintf(ret, format, ap);
        va_end(ap);
        return nc;
}

#endif /* _GNU_SOURCE */
