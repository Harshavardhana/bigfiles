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

#ifndef __COMPAT_H__
#define __COMPAT_H__

#include "config.h"

#ifndef HAVE_ASPRINTF
#include <stdarg.h>

static int vasprintf(char **ret, const char *format, va_list ap)
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

static int asprintf(char **ret, const char *format, ...)
{
        va_list ap;
        int nc;
        va_start (ap, format);
        nc = vasprintf(ret, format, ap);
        va_end(ap);
        return nc;
}

#endif /* HAVE_ASPRINTF */
#endif /* __COMPAT_H__ */
