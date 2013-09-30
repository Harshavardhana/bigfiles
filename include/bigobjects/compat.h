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

char *strndup (const char *, size_t);
int strcasecmp (const char *, const char *);
char *strchrnul (const char *, int);
void *mempcpy (void *, const void *, size_t);
int vasprintf (char **, const char *, va_list);
int asprintf (char **, const char *, ...);

#endif /* __COMPAT_H__ */
