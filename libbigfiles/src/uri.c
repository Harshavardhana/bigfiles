/**
 * uri.c: set of generic URI related routines
 *
 * Reference: RFCs 3986, 2732 and 2373
 *
 *  Copyright (C) 1998-2012 Daniel Veillard.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * DANIEL VEILLARD BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Daniel Veillard shall not
 * be used in advertising or otherwise to promote the sale, use or other
 * dealings in this Software without prior written authorization from him.
 *
 * daniel@veillard.com
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
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "uri.h"
#include "utils.h"

/**
 * bigfile_rfc3986_scheme:
 * @uri:  pointer to an URI structure
 * @str:  pointer to the string to analyze
 *
 * Parse an URI scheme
 *
 * ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
 *
 * Returns 0 or the error code
 */

static int
bigfile_rfc3986_scheme(bURI *uri, const char **str)
{
        const char *cur = NULL;

        if (str == NULL)
                return -1;
        cur = *str;
        if (!ISA_ALPHA(cur))
                return 2;
        cur++;
        while (ISA_ALPHA(cur) || ISA_DIGIT(cur) ||
               (*cur == '+') || (*cur == '-') || (*cur == '.'))
                cur++;
        if (uri != NULL) {
                if (uri->scheme != NULL)
                        free(uri->scheme);
                uri->scheme = strndup(*str, cur - *str);
        }
        *str = cur;
        return 0;
}

/**
 * bigfile_rfc3986_fragment:
 * @uri:  pointer to an URI structure
 * @str:  pointer to the string to analyze
 *
 * Parse the query part of an URI
 *
 * fragment      = *( pchar / "/" / "?" )
 * NOTE: the strict syntax as defined by 3986 does not allow '[' and ']'
 *       in the fragment identifier but this is used very broadly for
 *       xpointer scheme selection, so we are allowing it here to not break
 *       for example all the DocBook processing chains.
 *
 * Returns 0 or the error code
 */
static int
bigfile_rfc3986_fragment(bURI *uri, const char **str)
{
        const char *cur = NULL;

        if (str == NULL)
                return -1;

        cur = *str;

        while ((ISA_PCHAR(cur)) || (*cur == '/') || (*cur == '?') ||
               (*cur == '[') || (*cur == ']') ||
               ((uri != NULL) && (uri->cleanup & 1) && (IS_UNWISE(cur))))
                NEXT(cur);
        if (uri != NULL) {
                if (uri->fragment != NULL)
                        free(uri->fragment);
                if (uri->cleanup & 2)
                        uri->fragment = strndup(*str, cur - *str);
                else
                        uri->fragment = bigfile_uri_string_unescape(*str,
                                                                    cur - *str,
                                                                    NULL);
        }

        *str = cur;
        return 0;
}

/**
 * bigfile_rfc3986_query:
 * @uri:  pointer to an URI structure
 * @str:  pointer to the string to analyze
 *
 * Parse the query part of an URI
 *
 * query = *uric
 *
 * Returns 0 or the error code
 */
static int
bigfile_rfc3986_query(bURI *uri, const char **str)
{
        const char *cur = NULL;

        if (str == NULL)
                return -1;

        cur = *str;

        while ((ISA_PCHAR(cur)) || (*cur == '/') || (*cur == '?') ||
               ((uri != NULL) && (uri->cleanup & 1) && (IS_UNWISE(cur))))
                NEXT(cur);
        if (uri != NULL) {
                if (uri->query != NULL)
                        free(uri->query);
                if (uri->cleanup & 2)
                        uri->query = strndup(*str, cur - *str);
                else
                        uri->query = bigfile_uri_string_unescape(*str, cur - *str,
                                                         NULL);
                /* Save the raw bytes of the query as well.
                 * See: http://mail.gnome.org/archives/xml/2007-April/thread.html#00114
                 */
                if (uri->query_raw != NULL)
                        free(uri->query_raw);
                uri->query_raw = strndup (*str, cur - *str);
        }
        *str = cur;
        return 0;
}

/**
 * bigfile_rfc3986_port:
 * @uri:  pointer to an URI structure
 * @str:  the string to analyze
 *
 * Parse a port  part and fills in the appropriate fields
 * of the @uri structure
 *
 * port          = *DIGIT
 *
 * Returns 0 or the error code
 */
static int
bigfile_rfc3986_port(bURI *uri, const char **str)
{
        const char *cur = NULL;
        if (str == NULL)
                return -1;

        cur = *str;

        if (ISA_DIGIT(cur)) {
                if (uri != NULL)
                        uri->port = 0;
                while (ISA_DIGIT(cur)) {
                        if (uri != NULL)
                                uri->port = uri->port * 10 + (*cur - '0');
                        cur++;
                }
                *str = cur;
                return 0;
        }
        return 1;
}

/**
 * bigfile_rfc3986_user_info:
 * @uri:  pointer to an URI structure
 * @str:  the string to analyze
 *
 * Parse an user informations part and fills in the appropriate fields
 * of the @uri structure
 *
 * userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
 *
 * Returns 0 or the error code
 */

static int
bigfile_rfc3986_user_info(bURI *uri, const char **str)
{
        const char *cur = NULL;

        if (str == NULL)
                return -1;

        cur = *str;
        while (ISA_UNRESERVED(cur) || ISA_PCT_ENCODED(cur) ||
               ISA_SUB_DELIM(cur) || (*cur == ':'))
                NEXT(cur);
        if (*cur == '@') {
                if (uri != NULL) {
                        if (uri->user != NULL) free(uri->user);
                        if (uri->cleanup & 2)
                                uri->user = strndup(*str, cur - *str);
                        else
                                uri->user = bigfile_uri_string_unescape(*str,
                                                                cur - *str,
                                                                NULL);
                }
                *str = cur;
                return 0;
        }
        return 1;
}

/**
 * bigfile_rfc3986_dec_octet:
 * @str:  the string to analyze
 *
 *    dec-octet     = DIGIT                 ; 0-9
 *                  / %x31-39 DIGIT         ; 10-99
 *                  / "1" 2DIGIT            ; 100-199
 *                  / "2" %x30-34 DIGIT     ; 200-249
 *                  / "25" %x30-35          ; 250-255
 *
 * Skip a dec-octet.
 *
 * Returns 0 if found and skipped, 1 otherwise
 */
static int
bigfile_rfc3986_dec_octet(const char **str)
{
        const char *cur = NULL;
        if (str == NULL)
                return -1;
        cur = *str;

        if (!(ISA_DIGIT(cur)))
                return 1;
        if (!ISA_DIGIT(cur+1))
                cur++;
        else if ((*cur != '0') && (ISA_DIGIT(cur + 1)) && (!ISA_DIGIT(cur+2)))
                cur += 2;
        else if ((*cur == '1') && (ISA_DIGIT(cur + 1)) && (ISA_DIGIT(cur + 2)))
                cur += 3;
        else if ((*cur == '2') && (*(cur + 1) >= '0') &&
                 (*(cur + 1) <= '4') && (ISA_DIGIT(cur + 2)))
                cur += 3;
        else if ((*cur == '2') && (*(cur + 1) == '5') &&
                 (*(cur + 2) >= '0') && (*(cur + 1) <= '5'))
                cur += 3;
        else
                return 1;
        *str = cur;
        return 0;
}
/**
 * bigfile_rfc3986_host:
 * @uri:  pointer to an URI structure
 * @str:  the string to analyze
 *
 * Parse an host part and fills in the appropriate fields
 * of the @uri structure
 *
 * host          = IP-literal / IPv4address / reg-name
 * IP-literal    = "[" ( IPv6address / IPvFuture  ) "]"
 * IPv4address   = dec-octet "." dec-octet "." dec-octet "." dec-octet
 * reg-name      = *( unreserved / pct-encoded / sub-delims )
 *
 * Returns 0 or the error code
 */
static int
bigfile_rfc3986_host(bURI *uri, const char **str)
{
        const char *cur  = NULL;
        const char *host = NULL;

        if (str == NULL)
                return -1;

        cur = *str;
        if (cur == NULL)
                return -1;

        host = cur;
        /*
         * IPv6 and future addressing scheme are enclosed between brackets
         */
        if (*cur == '[') {
                cur++;
                while ((*cur != ']') && (*cur != 0))
                        cur++;
                if (*cur != ']')
                        return 1;
                cur++;
                goto found;
        }
        /*
         * try to parse an IPv4
         */
        if (ISA_DIGIT(cur)) {
                if (bigfile_rfc3986_dec_octet(&cur) != 0)
                        goto notipv4;
                if (*cur != '.')
                        goto notipv4;
                cur++;
                if (bigfile_rfc3986_dec_octet(&cur) != 0)
                        goto notipv4;
                if (*cur != '.')
                        goto notipv4;
                if (bigfile_rfc3986_dec_octet(&cur) != 0)
                        goto notipv4;
                if (*cur != '.')
                        goto notipv4;
                if (bigfile_rfc3986_dec_octet(&cur) != 0)
                        goto notipv4;
                goto found;
        notipv4:
                cur = *str;
        }
        /*
         * then this should be a hostname which can be empty
         */
        while (ISA_UNRESERVED(cur) || ISA_PCT_ENCODED(cur)
               || ISA_SUB_DELIM(cur))
                NEXT(cur);
found:
        if (uri != NULL) {
                if (uri->authority != NULL)
                        free(uri->authority);
                uri->authority = NULL;
                if (uri->server != NULL)
                        free(uri->server);
                if (cur != host) {
                        if (uri->cleanup & 2)
                                uri->server = strndup(host, cur - host);
                        else
                                uri->server = bigfile_uri_string_unescape(host,
                                                                  cur - host,
                                                                  NULL);
                } else
                        uri->server = NULL;
        }
        *str = cur;
        return 0;
}

/**
 * bigfile_rfc3986_authority:
 * @uri:  pointer to an URI structure
 * @str:  the string to analyze
 *
 * Parse an authority part and fills in the appropriate fields
 * of the @uri structure
 *
 * authority     = [ userinfo "@" ] host [ ":" port ]
 *
 * Returns 0 or the error code
 */
static int
bigfile_rfc3986_authority(bURI *uri, const char **str)
{
        const char *cur = NULL;
        int ret         = 0;

        if (str == NULL)
                return -1;

        cur = *str;

        ret = bigfile_rfc3986_user_info(uri, &cur);

        if ((ret != 0) || (*cur != '@'))
                cur = *str;
        else
                cur++;
        if (bigfile_rfc3986_host(uri, &cur) != 0)
                return 1;
        if (*cur == ':') {
                cur++;
                if (bigfile_rfc3986_port(uri, &cur) != 0)
                        return 1;
        }
        *str = cur;
        return 0;
}

/**
 * bigfile_rfc3986_segment:
 * @str:  the string to analyze
 * @forbid: an optional forbidden character
 * @empty: allow an empty segment
 *
 * Parse a segment and fills in the appropriate fields
 * of the @uri structure
 *
 * segment       = *pchar
 * segment-nz    = 1*pchar
 * segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
 *               ; non-zero-length segment without any colon ":"
 *
 * Returns 0 or the error code
 */
static int
bigfile_rfc3986_segment(const char **str, char forbid, int empty)
{
        const char *cur = NULL;
        int ret         = 0;

        if (str == NULL) {
                ret = -1;
                goto out;
        }
        cur = *str;
        if (!ISA_PCHAR(cur)) {
                if (!empty)
                        ret = 1;
                goto out;
        }
        while (ISA_PCHAR(cur) && (*cur != forbid))
                NEXT(cur);
        *str = cur;
out:
        return ret;
}

/**
 * bigfile_rfc3986_path_ab_empty:
 * @uri:  pointer to an URI structure
 * @str:  the string to analyze
 *
 * Parse an path absolute or empty and fills in the appropriate fields
 * of the @uri structure
 *
 * path-abempty  = *( "/" segment )
 *
 * Returns 0 or the error code
 */
static int
bigfile_rfc3986_path_ab_empty(bURI *uri, const char **str)
{
        const char *cur = NULL;
        int ret         = 0;

        if (str == NULL) {
                ret = -1;
                goto out;
        }

        cur = *str;

        while (*cur == '/') {
                cur++;
                ret = bigfile_rfc3986_segment(&cur, 0, 1);
                if (ret != 0)
                        goto out;
        }
        if (uri != NULL) {
                if (uri->path != NULL)
                        free(uri->path);
                if (*str != cur) {
                        if (uri->cleanup & 2)
                                uri->path = strndup(*str, cur - *str);
                        else
                                uri->path = bigfile_uri_string_unescape(*str,
                                                                cur - *str,
                                                                NULL);
                } else {
                        uri->path = NULL;
                }
        }
        *str = cur;
out:
        return ret;
}

/**
 * bigfile_rfc3986_path_absolute:
 * @uri:  pointer to an URI structure
 * @str:  the string to analyze
 *
 * Parse an path absolute and fills in the appropriate fields
 * of the @uri structure
 *
 * path-absolute = "/" [ segment-nz *( "/" segment ) ]
 *
 * Returns 0 or the error code
 */
static int
bigfile_rfc3986_path_absolute(bURI *uri, const char **str)
{
        const char *cur = NULL;
        int ret         = 0;

        if (str == NULL) {
                ret = -1;
                goto out;
        }

        cur = *str;
        if (*cur != '/') {
                ret = 1;
                goto out;
        }

        cur++;
        ret = bigfile_rfc3986_segment(&cur, 0, 0);
        if (ret == 0) {
                while (*cur == '/') {
                        cur++;
                        ret = bigfile_rfc3986_segment(&cur, 0, 1);
                        if (ret != 0)
                                goto out;
                }
        }
        if (uri != NULL) {
                if (uri->path != NULL)
                        free(uri->path);
                if (cur != *str) {
                        if (uri->cleanup & 2)
                                uri->path = strndup(*str, cur - *str);
                        else
                                uri->path = bigfile_uri_string_unescape(*str,
                                                                cur - *str,
                                                                NULL);
                } else {
                        uri->path = NULL;
                }
        }
        *str = cur;
out:
        return ret;
}

/**
 * bigfile_rfc3986_path_rootless:
 * @uri:  pointer to an URI structure
 * @str:  the string to analyze
 *
 * Parse an path without root and fills in the appropriate fields
 * of the @uri structure
 *
 * path-rootless = segment-nz *( "/" segment )
 *
 * Returns 0 or the error code
 */
static int
bigfile_rfc3986_path_rootless(bURI *uri, const char **str)
{
        const char *cur;
        int ret;

        cur = *str;

        ret = bigfile_rfc3986_segment(&cur, 0, 0);
        if (ret != 0)
                return ret;
        while (*cur == '/') {
                cur++;
                ret = bigfile_rfc3986_segment(&cur, 0, 1);
                if (ret != 0)
                        return ret;
        }
        if (uri != NULL) {
                if (uri->path != NULL) free(uri->path);
                if (cur != *str) {
                        if (uri->cleanup & 2)
                                uri->path = strndup(*str, cur - *str);
                        else
                                uri->path = bigfile_uri_string_unescape(*str,
                                                                cur - *str,
                                                                NULL);
                } else {
                        uri->path = NULL;
                }
        }
        *str = cur;
        return 0;
}

/**
 * bigfile_rfc3986_path_no_scheme:
 * @uri:  pointer to an URI structure
 * @str:  the string to analyze
 *
 * Parse an path which is not a scheme and fills in the appropriate fields
 * of the @uri structure
 *
 * path-noscheme = segment-nz-nc *( "/" segment )
 *
 * Returns 0 or the error code
 */
static int
bigfile_rfc3986_path_no_scheme(bURI *uri, const char **str)
{
        const char *cur;
        int ret;

        cur = *str;

        ret = bigfile_rfc3986_segment(&cur, ':', 0);
        if (ret != 0)
                return ret;

        while (*cur == '/') {
                cur++;
                ret = bigfile_rfc3986_segment(&cur, 0, 1);
                if (ret != 0)
                        return ret;
        }
        if (uri != NULL) {
                if (uri->path != NULL)
                        free(uri->path);
                if (cur != *str) {
                        if (uri->cleanup & 2)
                                uri->path = strndup(*str, cur - *str);
                        else
                                uri->path = bigfile_uri_string_unescape(*str,
                                                                cur - *str,
                                                                NULL);
                } else {
                        uri->path = NULL;
                }
        }
        *str = cur;
        return 0;
}

/**
 * bigfile_rfc3986_hier_part:
 * @uri:  pointer to an URI structure
 * @str:  the string to analyze
 *
 * Parse an hierarchical part and fills in the appropriate fields
 * of the @uri structure
 *
 * hier-part     = "//" authority path-abempty
 *                / path-absolute
 *                / path-rootless
 *                / path-empty
 *
 * Returns 0 or the error code
 */
static int
bigfile_rfc3986_hier_part(bURI *uri, const char **str)
{
        const char *cur;
        int ret;

        cur = *str;

        if ((*cur == '/') && (*(cur + 1) == '/')) {
                cur += 2;
                ret = bigfile_rfc3986_authority(uri, &cur);
                if (ret != 0) return ret;
                ret = bigfile_rfc3986_path_ab_empty(uri, &cur);
                if (ret != 0) return ret;
                *str = cur;
                return 0;
        } else if (*cur == '/') {
                ret = bigfile_rfc3986_path_absolute(uri, &cur);
                if (ret != 0) return ret;
        } else if (ISA_PCHAR(cur)) {
                ret = bigfile_rfc3986_path_rootless(uri, &cur);
                if (ret != 0) return ret;
        } else {
                /* path-empty is effectively empty */
                if (uri != NULL) {
                        if (uri->path != NULL) free(uri->path);
                        uri->path = NULL;
                }
        }
        *str = cur;
        return 0;
}

/**
 * bigfile_rfc3986_relative_ref:
 * @uri:  pointer to an URI structure
 * @str:  the string to analyze
 *
 * Parse an URI string and fills in the appropriate fields
 * of the @uri structure
 *
 * relative-ref  = relative-part [ "?" query ] [ "#" fragment ]
 * relative-part = "//" authority path-abempty
 *               / path-absolute
 *               / path-noscheme
 *               / path-empty
 *
 * Returns 0 or the error code
 */
static int
bigfile_rfc3986_relative_ref(bURI *uri, const char *str)
{
        int ret;

        if ((*str == '/') && (*(str + 1) == '/')) {
                str += 2;
                ret = bigfile_rfc3986_authority(uri, &str);
                if (ret != 0) return(ret);
                ret = bigfile_rfc3986_path_ab_empty(uri, &str);
                if (ret != 0) return(ret);
        } else if (*str == '/') {
                ret = bigfile_rfc3986_path_absolute(uri, &str);
                if (ret != 0) return(ret);
        } else if (ISA_PCHAR(str)) {
                ret = bigfile_rfc3986_path_no_scheme(uri, &str);
                if (ret != 0) return(ret);
        } else {
                /* path-empty is effectively empty */
                if (uri != NULL) {
                        if (uri->path != NULL) free(uri->path);
                        uri->path = NULL;
                }
        }

        if (*str == '?') {
                str++;
                ret = bigfile_rfc3986_query(uri, &str);
                if (ret != 0)
                        return ret;
        }
        if (*str == '#') {
                str++;
                ret = bigfile_rfc3986_fragment(uri, &str);
                if (ret != 0)
                        return ret;
        }
        if (*str != 0) {
                BF_URI_TRIM(uri);
                return 1;
        }
        return 0;
}


/**
 * bigfile_rfc3986:
 * @uri:  pointer to an URI structure
 * @str:  the string to analyze
 *
 * Parse an URI string and fills in the appropriate fields
 * of the @uri structure
 *
 * scheme ":" hier-part [ "?" query ] [ "#" fragment ]
 *
 * Returns 0 or the error code
 */
static int
bigfile_rfc3986(bURI *uri, const char *str)
{
        int ret;

        ret = bigfile_rfc3986_scheme(uri, &str);
        if (ret != 0)
                return ret;
        if (*str != ':')
                return 1;
        str++;

        ret = bigfile_rfc3986_hier_part(uri, &str);
        if (ret != 0)
                return ret;
        if (*str == '?') {
                str++;
                ret = bigfile_rfc3986_query(uri, &str);
                if (ret != 0)
                        return ret;
        }
        if (*str == '#') {
                str++;
                ret = bigfile_rfc3986_fragment(uri, &str);
                if (ret != 0)
                        return ret;
        }
        if (*str != 0) {
                BF_URI_TRIM(uri);
                return 1;
        }
        return 0;
}

/**
 * bigfile_rfc3986_uri_reference:
 * @uri:  pointer to an URI structure
 * @str:  the string to analyze
 *
 * Parse an URI reference string and fills in the appropriate fields
 * of the @uri structure
 *
 * URI-reference = URI / relative-ref
 *
 * Returns 0 or the error code
 */
static int
bigfile_rfc3986_uri_reference(bURI *uri, const char *str)
{
        int ret;

        if (str == NULL)
                return -1;

        BF_URI_TRIM(uri);
        /*
         * Try first to parse absolute refs, then fallback to relative if
         * it fails.
         */
        ret = bigfile_rfc3986(uri, str);
        if (ret != 0) {
                BF_URI_TRIM(uri);
                ret = bigfile_rfc3986_relative_ref(uri, str);
                if (ret != 0) {
                        BF_URI_TRIM(uri);
                        return ret;
                }
        }
        return 0;
}


/************************************************************************
 *                Generic URI structure functions                       *
 ************************************************************************/

/**
 * bigfile_uri_parse_into:
 * @uri:  pointer to an URI structure
 * @str:  the string to analyze
 *
 * Parse an URI reference string based on RFC 3986 and fills in the
 * appropriate fields of the @uri structure
 *
 * URI-reference = URI / relative-ref
 *
 * Returns 0 or the error code
 */
int
bigfile_uri_parse_into(bURI *uri, const char *str)
{
        return (bigfile_rfc3986_uri_reference(uri, str));
}

/**
 * bigfile_uri_parse:
 * @str:  the bURI string to analyze
 *
 * Parse an bURI based on RFC 3986
 *
 * URI-reference = [ absoluteURI | relativeURI ] [ "#" fragment ]
 *
 * Returns a newly built bURI or NULL in case of error
 */
bURI *
bigfile_uri_parse(const char *str)
{
        bURI *uri = NULL;

        if (str == NULL)
                goto out;

        uri = bigfile_uri_new();
        if (uri != NULL) {
                if (bigfile_uri_parse_into(uri, str)) {
                        BF_URI_FREE(uri);
                        goto out;
                }
        }
out:
        return uri;
}

/**
 * bigfile_uri_parse_raw:
 * @str:  the bURI string to analyze
 * @raw:  if 1 unescaping of bURI pieces are disabled
 *
 * Parse an bURI but allows to keep intact the original fragments.
 *
 * bURI-reference = bURI / relative-ref
 *
 * Returns a newly built URI or NULL in case of error
 */
bURI *
bigfile_uri_parse_raw(const char *str, int raw)
{
        bURI *uri = NULL;

        if (str == NULL)
                goto out;
        uri = bigfile_uri_new();
        if (uri != NULL) {
                if (raw) {
                        uri->cleanup |= 2;
                }
                if (bigfile_uri_parse_into(uri, str)) {
                        BF_URI_FREE(uri);
                        goto out;
                }
        }
out:
        return uri;
}

/**
 * bigfile_uri_new:
 *
 * Simply creates an empty URI
 *
 * Returns the new structure or NULL in case of error
 */
bURI *
bigfile_uri_new(void)
{
        bURI *tmp_uri;
        tmp_uri = (bURI *) malloc(sizeof(bURI));
        memset(tmp_uri, 0, sizeof(bURI));
        return tmp_uri;
}

/**
 * realloc2n:
 *
 * Function to handle properly a reallocation when saving an URI
 * Also imposes some limit on the length of an URI string output
 */
static char *
realloc2n(char *ret, int *max)
{
        char *temp;
        int tmp;

        tmp = *max * 2;
        temp = realloc(ret, (tmp + 1));
        *max = tmp;
        return temp;
}

/**
 * bigfile_uri_to_string:
 * @uri:  pointer to an bURI
 *
 * Save the bURI as an escaped string
 *
 * Returns a new string (to be deallocated by caller)
 */
char *
bigfile_uri_to_string(bURI *uri)
{
        char *ret = NULL;
        char *temp;
        const char *p;
        int len;
        int max;

        if (uri == NULL)
                return NULL;

        max = 80;
        ret = malloc(max + 1);
        len = 0;

        if (uri->scheme != NULL) {
                p = uri->scheme;
                while (*p != 0) {
                        if (len >= max) {
                                temp = realloc2n(ret, &max);
                                if (temp == NULL) goto mem_error;
                                ret = temp;
                        }
                        ret[len++] = *p++;
                }
                if (len >= max) {
                        temp = realloc2n(ret, &max);
                        if (temp == NULL) goto mem_error;
                        ret = temp;
                }
                ret[len++] = ':';
        }
        if (uri->opaque != NULL) {
                p = uri->opaque;
                while (*p != 0) {
                        if (len + 3 >= max) {
                                temp = realloc2n(ret, &max);
                                if (temp == NULL) goto mem_error;
                                ret = temp;
                        }
                        if (IS_RESERVED(*(p)) || IS_UNRESERVED(*(p)))
                                ret[len++] = *p++;
                        else {
                                int val = *(unsigned char *)p++;
                                int hi = val / 0x10, lo = val % 0x10;
                                ret[len++] = '%';
                                ret[len++] = hi + (hi > 9? 'A'-10 : '0');
                                ret[len++] = lo + (lo > 9? 'A'-10 : '0');
                        }
                }
        } else {
                if (uri->server != NULL) {
                        if (len + 3 >= max) {
                                temp = realloc2n(ret, &max);
                                if (temp == NULL) goto mem_error;
                                ret = temp;
                        }
                        ret[len++] = '/';
                        ret[len++] = '/';
                        if (uri->user != NULL) {
                                p = uri->user;
                                while (*p != 0) {
                                        if (len + 3 >= max) {
                                                temp = realloc2n(ret, &max);
                                                if (temp == NULL)
                                                        goto mem_error;
                                                ret = temp;
                                        }
                                        if ((IS_UNRESERVED(*(p))) ||
                                            ((*(p) == ';')) ||
                                            ((*(p) == ':')) ||
                                            ((*(p) == '&')) ||
                                            ((*(p) == '=')) ||
                                            ((*(p) == '+')) ||
                                            ((*(p) == '$')) ||
                                            ((*(p) == ',')))
                                                ret[len++] = *p++;
                                        else {
                                                int val = *(unsigned char *)p++;
                                                int hi = val / 0x10;
                                                int lo = val % 0x10;
                                                ret[len++] = '%';
                                                ret[len++] = hi +
                                                        (hi > 9? 'A'-10 : '0');
                                                ret[len++] = lo +
                                                        (lo > 9? 'A'-10 : '0');
                                        }
                                }
                                if (len + 3 >= max) {
                                        temp = realloc2n(ret, &max);
                                        if (temp == NULL)
                                                goto mem_error;
                                        ret = temp;
                                }
                                ret[len++] = '@';
                        }
                        p = uri->server;
                        while (*p != 0) {
                                if (len >= max) {
                                        temp = realloc2n(ret, &max);
                                        if (temp == NULL)
                                                goto mem_error;
                                        ret = temp;
                                }
                                ret[len++] = *p++;
                        }
                        if (uri->port > 0) {
                                if (len + 10 >= max) {
                                        temp = realloc2n(ret, &max);
                                        if (temp == NULL) goto mem_error;
                                        ret = temp;
                                }
                                len += snprintf(&ret[len], max - len, ":%d",
                                                uri->port);
                        }
                } else if (uri->authority != NULL) {
                        if (len + 3 >= max) {
                                temp = realloc2n(ret, &max);
                                if (temp == NULL) goto mem_error;
                                ret = temp;
                        }
                        ret[len++] = '/';
                        ret[len++] = '/';
                        p = uri->authority;
                        while (*p != 0) {
                                if (len + 3 >= max) {
                                        temp = realloc2n(ret, &max);
                                        if (temp == NULL) goto mem_error;
                                        ret = temp;
                                }
                                if ((IS_UNRESERVED(*(p))) ||
                                    ((*(p) == '$')) || ((*(p) == ',')) ||
                                    ((*(p) == ';')) ||
                                    ((*(p) == ':')) || ((*(p) == '@')) ||
                                    ((*(p) == '&')) ||
                                    ((*(p) == '=')) || ((*(p) == '+')))
                                        ret[len++] = *p++;
                                else {
                                        int val = *(unsigned char *)p++;
                                        int hi = val / 0x10, lo = val % 0x10;
                                        ret[len++] = '%';
                                        ret[len++] = hi +
                                                (hi > 9? 'A'-10 : '0');
                                        ret[len++] = lo +
                                                (lo > 9? 'A'-10 : '0');
                                }
                        }
                } else if (uri->scheme != NULL) {
                        if (len + 3 >= max) {
                                temp = realloc2n(ret, &max);
                                if (temp == NULL) goto mem_error;
                                ret = temp;
                        }
                        ret[len++] = '/';
                        ret[len++] = '/';
                }
                if (uri->path != NULL) {
                        p = uri->path;
                        /*
                         * the colon in file:///d: should not be escaped or
                         * Windows accesses fail later.
                         */
                        if ((uri->scheme != NULL) &&
                            (p[0] == '/') &&
                            (((p[1] >= 'a') && (p[1] <= 'z')) ||
                             ((p[1] >= 'A') && (p[1] <= 'Z'))) &&
                            (p[2] == ':') &&
                            (!strcmp(uri->scheme, "file"))) {
                                if (len + 3 >= max) {
                                        temp = realloc2n(ret, &max);
                                        if (temp == NULL) goto mem_error;
                                        ret = temp;
                                }
                                ret[len++] = *p++;
                                ret[len++] = *p++;
                                ret[len++] = *p++;
                        }
                        while (*p != 0) {
                                if (len + 3 >= max) {
                                        temp = realloc2n(ret, &max);
                                        if (temp == NULL) goto mem_error;
                                        ret = temp;
                                }
                                if ((IS_UNRESERVED(*(p))) || ((*(p) == '/')) ||
                                    ((*(p) == ';')) || ((*(p) == '@')) ||
                                    ((*(p) == '&')) ||
                                    ((*(p) == '=')) || ((*(p) == '+')) ||
                                    ((*(p) == '$')) ||
                                    ((*(p) == ',')))
                                        ret[len++] = *p++;
                                else {
                                        int val = *(unsigned char *)p++;
                                        int hi = val / 0x10, lo = val % 0x10;
                                        ret[len++] = '%';
                                        ret[len++] = hi +
                                                (hi > 9? 'A'-10 : '0');
                                        ret[len++] = lo +
                                                (lo > 9? 'A'-10 : '0');
                                }
                        }
                }
                if (uri->query != NULL) {
                        if (len + 1 >= max) {
                                temp = realloc2n(ret, &max);
                                if (temp == NULL) goto mem_error;
                                ret = temp;
                        }
                        ret[len++] = '?';
                        p = uri->query;
                        while (*p != 0) {
                                if (len + 1 >= max) {
                                        temp = realloc2n(ret, &max);
                                        if (temp == NULL) goto mem_error;
                                        ret = temp;
                                }
                                ret[len++] = *p++;
                        }
                }
        }
        if (uri->fragment != NULL) {
                if (len + 3 >= max) {
                        temp = realloc2n(ret, &max);
                        if (temp == NULL) goto mem_error;
                        ret = temp;
                }
                ret[len++] = '#';
                p = uri->fragment;
                while (*p != 0) {
                        if (len + 3 >= max) {
                                temp = realloc2n(ret, &max);
                                if (temp == NULL) goto mem_error;
                                ret = temp;
                        }
                        if ((IS_UNRESERVED(*(p))) || (IS_RESERVED(*(p))))
                                ret[len++] = *p++;
                        else {
                                int val = *(unsigned char *)p++;
                                int hi = val / 0x10, lo = val % 0x10;
                                ret[len++] = '%';
                                ret[len++] = hi + (hi > 9? 'A'-10 : '0');
                                ret[len++] = lo + (lo > 9? 'A'-10 : '0');
                        }
                }
        }
        if (len >= max) {
                temp = realloc2n(ret, &max);
                if (temp == NULL)
                        goto mem_error;
                ret = temp;
        }
        ret[len] = 0;
        return ret;

mem_error:
        FREE(ret);

        return NULL;
}


/**
 * bigfile_uri_string_unescape:
 * @str:  the string to unescape
 * @len:   the length in bytes to unescape (or <= 0 to indicate full string)
 * @target:  optional destination buffer
 *
 * Unescaping routine, but does not check that the string is an URI. The
 * output is a direct unsigned char translation of %XX values (no encoding)
 * Note that the length of the result can only be smaller or same size as
 * the input string.
 *
 * Returns a copy of the string, but unescaped, will return NULL only in case
 * of error
 */
char *
bigfile_uri_string_unescape(const char *str, int len, char *target)
{
        char *ret, *out;
        const char *in;

        if (str == NULL)
                return(NULL);
        if (len <= 0)
                len = strlen(str);
        if (len < 0)
                return(NULL);
        if (target == NULL) {
                ret = malloc(len + 1);
        } else
                ret = target;
        in = str;
        out = ret;
        while(len > 0) {
                if ((len > 2) && (*in == '%') &&
                    (is_hex(in[1])) && (is_hex(in[2]))) {
                        in++;
                        if ((*in >= '0') && (*in <= '9'))
                                *out = (*in - '0');
                        else if ((*in >= 'a') && (*in <= 'f'))
                                *out = (*in - 'a') + 10;
                        else if ((*in >= 'A') && (*in <= 'F'))
                                *out = (*in - 'A') + 10;
                        in++;
                        if ((*in >= '0') && (*in <= '9'))
                                *out = *out * 16 + (*in - '0');
                        else if ((*in >= 'a') && (*in <= 'f'))
                                *out = *out * 16 + (*in - 'a') + 10;
                        else if ((*in >= 'A') && (*in <= 'F'))
                                *out = *out * 16 + (*in - 'A') + 10;
                        in++;
                        len -= 3;
                        out++;
                } else {
                        *out++ = *in++;
                        len--;
                }
        }
        *out = 0;
        return ret;
}

/**
 * bigfile_uri_string_escape:
 * @str:  string to escape
 * @list: exception list string of chars not to escape
 *
 * This routine escapes a string to hex, ignoring reserved characters (a-z)
 * and the characters in the exception list.
 *
 * Returns a new escaped string or NULL in case of error.
 */
char *
bigfile_uri_string_escape(const char *str, const char *list)
{
        char *ret, ch;
        char *temp;
        const char *in;
        int len, out;

        if (str == NULL)
                return(NULL);
        if (str[0] == 0)
                return(strdup(str));
        len = strlen(str);
        if (!(len > 0)) return(NULL);

        len += 20;
        ret = malloc(len);
        in = str;
        out = 0;
        while(*in != 0) {
                if (len - out <= 3) {
                        temp = realloc2n(ret, &len);
                        ret = temp;
                }
                ch = *in;

                if ((ch != '@') && (!IS_UNRESERVED(ch)) &&
                    (!strchr(list, ch))) {
                        unsigned char val;
                        ret[out++] = '%';
                        val = ch >> 4;
                        if (val <= 9)
                                ret[out++] = '0' + val;
                        else
                                ret[out++] = 'A' + val - 0xA;
                        val = ch & 0xF;
                        if (val <= 9)
                                ret[out++] = '0' + val;
                        else
                                ret[out++] = 'A' + val - 0xA;
                        in++;
                } else {
                        ret[out++] = *in++;
                }
        }
        ret[out] = 0;
        return(ret);
}

/************************************************************************
 *									*
 *			Public functions				*
 *									*
 ************************************************************************/

/**
 * bigfile_uri_resolve:
 * @URI:  the URI instance found in the document
 * @base:  the base value
 *
 * Computes he final URI of the reference done by checking that
 * the given URI is valid, and building the final URI using the
 * base URI. This is processed according to section 5.2 of the
 * RFC 2396
 *
 * 5.2. Resolving Relative References to Absolute Form
 *
 * Returns a new URI string (to be freed by the caller) or NULL in case
 *         of error.
 */
char *
bigfile_uri_resolve(const char *uri, const char *base)
{
        char *val = NULL;
        int ret, len, indx, cur, out;
        bURI *ref = NULL;
        bURI *bas = NULL;
        bURI *res = NULL;

        /*
         * 1) The URI reference is parsed into the potential four components and
         *    fragment identifier, as described in Section 4.3.
         *
         *    NOTE that a completely empty URI is treated by modern browsers
         *    as a reference to "." rather than as a synonym for the current
         *    URI.  Should we do that here?
         */
        if (uri == NULL)
                ret = -1;
        else {
                if (*uri) {
                        ref = bigfile_uri_new();
                        if (ref == NULL)
                                goto done;
                        ret = bigfile_uri_parse_into(ref, uri);
                }
                else
                        ret = 0;
        }
        if (ret != 0)
                goto done;
        if ((ref != NULL) && (ref->scheme != NULL)) {
                /*
                 * The URI is absolute don't modify.
                 */
                val = strdup(uri);
                goto done;
        }
        if (base == NULL)
                ret = -1;
        else {
                bas = bigfile_uri_new();
                if (bas == NULL)
                        goto done;
                ret = bigfile_uri_parse_into(bas, base);
        }
        if (ret != 0) {
                if (ref)
                        val = bigfile_uri_to_string(ref);
                goto done;
        }
        if (ref == NULL) {
                /*
                 * the base fragment must be ignored
                 */
                if (bas->fragment != NULL) {
                        free(bas->fragment);
                        bas->fragment = NULL;
                }
                val = bigfile_uri_to_string(bas);
                goto done;
        }

        /*
         * 2) If the path component is empty and the scheme, authority, and
         *    query components are undefined, then it is a reference to the
         *    current document and we are done.  Otherwise, the reference URI's
         *    query and fragment components are defined as found (or not found)
         *    within the URI reference and not inherited from the base URI.
         *
         *    NOTE that in modern browsers, the parsing differs from the above
         *    in the following aspect:  the query component is allowed to be
         *    defined while still treating this as a reference to the current
         *    document.
         */
        res = bigfile_uri_new();
        if (res == NULL)
                goto done;
        if ((ref->scheme == NULL) && (ref->path == NULL) &&
            ((ref->authority == NULL) && (ref->server == NULL))) {
                if (bas->scheme != NULL)
                        res->scheme = strdup(bas->scheme);
                if (bas->authority != NULL)
                        res->authority = strdup(bas->authority);
                else if (bas->server != NULL) {
                        res->server = strdup(bas->server);
                        if (bas->user != NULL)
                                res->user = strdup(bas->user);
                        res->port = bas->port;
                }
                if (bas->path != NULL)
                        res->path = strdup(bas->path);
                if (ref->query != NULL)
                        res->query = strdup (ref->query);
                else if (bas->query != NULL)
                        res->query = strdup(bas->query);
                if (ref->fragment != NULL)
                        res->fragment = strdup(ref->fragment);
                goto step_7;
        }

        /*
         * 3) If the scheme component is defined, indicating that the reference
         *    starts with a scheme name, then the reference is interpreted as an
         *    absolute URI and we are done.  Otherwise, the reference URI's
         *    scheme is inherited from the base URI's scheme component.
         */
        if (ref->scheme != NULL) {
                val = bigfile_uri_to_string(ref);
                goto done;
        }
        if (bas->scheme != NULL)
                res->scheme = strdup(bas->scheme);
        if (ref->query != NULL)
                res->query = strdup(ref->query);
        if (ref->fragment != NULL)
                res->fragment = strdup(ref->fragment);

        /*
         * 4) If the authority component is defined, then the reference is a
         *    network-path and we skip to step 7.  Otherwise, the reference
         *    URI's authority is inherited from the base URI's authority
         *    component, which will also be undefined if the URI scheme does not
         *    use an authority component.
         */
        if ((ref->authority != NULL) || (ref->server != NULL)) {
                if (ref->authority != NULL)
                        res->authority = strdup(ref->authority);
                else {
                        res->server = strdup(ref->server);
                        if (ref->user != NULL)
                                res->user = strdup(ref->user);
                        res->port = ref->port;
                }
                if (ref->path != NULL)
                        res->path = strdup(ref->path);
                goto step_7;
        }
        if (bas->authority != NULL)
                res->authority = strdup(bas->authority);
        else if (bas->server != NULL) {
                res->server = strdup(bas->server);
                if (bas->user != NULL)
                        res->user = strdup(bas->user);
                res->port = bas->port;
        }

        /*
         * 5) If the path component begins with a slash character ("/"), then
         *    the reference is an absolute-path and we skip to step 7.
         */
        if ((ref->path != NULL) && (ref->path[0] == '/')) {
                res->path = strdup(ref->path);
                goto step_7;
        }

        /*
         * 6) If this step is reached, then we are resolving a relative-path
         *    reference.  The relative path needs to be merged with the base
         *    URI's path.  Although there are many ways to do this, we will
         *    describe a simple method using a separate string buffer.
         *
         * Allocate a buffer large enough for the result string.
         */
        len = 2; /* extra / and 0 */
        if (ref->path != NULL)
                len += strlen(ref->path);
        if (bas->path != NULL)
                len += strlen(bas->path);
        res->path = malloc(len);
        res->path[0] = 0;

        /*
         * a) All but the last segment of the base URI's path component is
         *    copied to the buffer.  In other words, any characters after the
         *    last (right-most) slash character, if any, are excluded.
         */
        cur = 0;
        out = 0;
        if (bas->path != NULL) {
                while (bas->path[cur] != 0) {
                        while ((bas->path[cur] != 0) && (bas->path[cur] != '/'))
                                cur++;
                        if (bas->path[cur] == 0)
                                break;
                        cur++;
                        while (out < cur) {
                                res->path[out] = bas->path[out];
                                out++;
                        }
                }
        }
        res->path[out] = 0;

        /*
         * b) The reference's path component is appended to the buffer
         *    string.
         */
        if (ref->path != NULL && ref->path[0] != 0) {
                indx = 0;
                /*
                 * Ensure the path includes a '/'
                 */
                if ((out == 0) && (bas->server != NULL))
                        res->path[out++] = '/';
                while (ref->path[indx] != 0) {
                        res->path[out++] = ref->path[indx++];
                }
        }
        res->path[out] = 0;

        /*
         * Steps c) to h) are really path normalization steps
         */
        normalize_uri_path(res->path);

step_7:

        /*
         * 7) The resulting URI components, including any inherited from the
         *    base URI, are recombined to give the absolute form of the URI
         *    reference.
         */
        val = bigfile_uri_to_string(res);

done:
        if (ref != NULL)
                BF_URI_FREE(ref);
        if (bas != NULL)
                BF_URI_FREE(bas);
        if (res != NULL)
                BF_URI_FREE(res);
        return(val);
}

/**
 * bigfile_uri_resolve_relative:
 * @URI:  the URI reference under consideration
 * @base:  the base value
 *
 * Expresses the URI of the reference in terms relative to the
 * base.  Some examples of this operation include:
 *     base = "http://site1.com/docs/book1.html"
 *        URI input                        URI returned
 *     docs/pic1.gif                    pic1.gif
 *     docs/img/pic1.gif                img/pic1.gif
 *     img/pic1.gif                     ../img/pic1.gif
 *     http://site1.com/docs/pic1.gif   pic1.gif
 *     http://site2.com/docs/pic1.gif   http://site2.com/docs/pic1.gif
 *
 *     base = "docs/book1.html"
 *        URI input                        URI returned
 *     docs/pic1.gif                    pic1.gif
 *     docs/img/pic1.gif                img/pic1.gif
 *     img/pic1.gif                     ../img/pic1.gif
 *     http://site1.com/docs/pic1.gif   http://site1.com/docs/pic1.gif
 *
 *
 * Note: if the URI reference is really weird or complicated, it may be
 *       worthwhile to first convert it into a "nice" one by calling
 *       uri_resolve (using 'base') before calling this routine,
 *       since this routine (for reasonable efficiency) assumes URI has
 *       already been through some validation.
 *
 * Returns a new URI string (to be freed by the caller) or NULL in case
 * error.
 */
char *
bigfile_uri_resolve_relative (const char *uri, const char * base)
{
        char *val = NULL;
        int ret;
        int ix;
        int pos = 0;
        int nbslash = 0;
        int len;
        bURI *ref = NULL;
        bURI *bas = NULL;
        char *bptr, *uptr, *vptr;
        int remove_path = 0;

        if ((uri == NULL) || (*uri == 0))
                return NULL;

        /*
         * First parse URI into a standard form
         */
        ref = bigfile_uri_new ();
        if (ref == NULL)
                return NULL;
        /* If URI not already in "relative" form */
        if (uri[0] != '.') {
                ret = bigfile_uri_parse_into (ref, uri);
                if (ret != 0)
                        goto done; /* Error in URI, return NULL */
        } else
                ref->path = strdup(uri);

        /*
         * Next parse base into the same standard form
         */
        if ((base == NULL) || (*base == 0)) {
                val = strdup (uri);
                goto done;
        }
        bas = bigfile_uri_new ();
        if (bas == NULL)
                goto done;
        if (base[0] != '.') {
                ret = bigfile_uri_parse_into (bas, base);
                if (ret != 0)
                        goto done;    /* Error in base, return NULL */
        } else
                bas->path = strdup(base);
        /*
         * If the scheme / server on the URI differs from the base,
         * just return the URI
         */
        if ((ref->scheme != NULL) &&
            ((bas->scheme == NULL) ||
             (strcmp (bas->scheme, ref->scheme)) ||
             (strcmp (bas->server, ref->server)))) {
                val = strdup (uri);
                goto done;
        }
        if (!strcmp(bas->path, ref->path)) {
                val = strdup("");
                goto done;
        }
        if (bas->path == NULL) {
                val = strdup(ref->path);
                goto done;
        }
        if (ref->path == NULL) {
                ref->path = (char *) "/";
                remove_path = 1;
        }

        /*
         * At this point (at last!) we can compare the two paths
         *
         * First we take care of the special case where either of the
         * two path components may be missing (bug 316224)
         */
        if (bas->path == NULL) {
                if (ref->path != NULL) {
                        uptr = ref->path;
                        if (*uptr == '/')
                                uptr++;
                        /* exception characters from uri_to_string */
                        val = bigfile_uri_string_escape(uptr, "/;&=+$,");
                }
                goto done;
        }
        bptr = bas->path;
        if (ref->path == NULL) {
                for (ix = 0; bptr[ix] != 0; ix++) {
                        if (bptr[ix] == '/')
                                nbslash++;
                }
                uptr = NULL;
                len = 1;       /* this is for a string terminator only */
        } else {
                /*
                 * Next we compare the two strings and find where they first differ
                 */
                if ((ref->path[pos] == '.') && (ref->path[pos+1] == '/'))
                        pos += 2;
                if ((*bptr == '.') && (bptr[1] == '/'))
                        bptr += 2;
                else if ((*bptr == '/') && (ref->path[pos] != '/'))
                        bptr++;
                while ((bptr[pos] == ref->path[pos]) && (bptr[pos] != 0))
                        pos++;
                if (bptr[pos] == ref->path[pos]) {
                        val = strdup("");
                        goto done; /* (I can't imagine why anyone would do this) */
                }

                /*
                 * In URI, "back up" to the last '/' encountered.  This will be the
                 * beginning of the "unique" suffix of URI
                 */
                ix = pos;
                if ((ref->path[ix] == '/') && (ix > 0))
                        ix--;
                else if ((ref->path[ix] == 0) && (ix > 1) &&
                         (ref->path[ix - 1] == '/'))
                        ix -= 2;
                for (; ix > 0; ix--) {
                        if (ref->path[ix] == '/')
                                break;
                }
                if (ix == 0) {
                        uptr = ref->path;
                } else {
                        ix++;
                        uptr = &ref->path[ix];
                }

                /*
                 * In base, count the number of '/' from the differing point
                 */
                if (bptr[pos] != ref->path[pos]) {/* check for trivial URI == base */
                        for (; bptr[ix] != 0; ix++) {
                                if (bptr[ix] == '/')
                                        nbslash++;
                        }
                }
                len = strlen (uptr) + 1;
        }

        if (nbslash == 0) {
                if (uptr != NULL)
                        /* exception characters from uri_to_string */
                        val = bigfile_uri_string_escape(uptr, "/;&=+$,");
                goto done;
        }

        /*
         * Allocate just enough space for the returned string -
         * length of the remainder of the URI, plus enough space
         * for the "../" groups, plus one for the terminator
         */
        val = malloc (len + 3 * nbslash);
        vptr = val;
        /*
         * Put in as many "../" as needed
         */
        for (; nbslash>0; nbslash--) {
                *vptr++ = '.';
                *vptr++ = '.';
                *vptr++ = '/';
        }
        /*
         * Finish up with the end of the URI
         */
        if (uptr != NULL) {
                if ((vptr > val) && (len > 0) &&
                    (uptr[0] == '/') && (vptr[-1] == '/')) {
                        memcpy (vptr, uptr + 1, len - 1);
                        vptr[len - 2] = 0;
                } else {
                        memcpy (vptr, uptr, len);
                        vptr[len - 1] = 0;
                }
        } else {
                vptr[len - 1] = 0;
        }

        /* escape the freshly-built path */
        vptr = val;
        /* exception characters from uri_to_string */
        val = bigfile_uri_string_escape(vptr, "/;&=+$,");
        free(vptr);

done:
        /*
         * Free the working variables
         */
        if (remove_path != 0)
                ref->path = NULL;
        if (ref != NULL)
                BF_URI_FREE (ref);
        if (bas != NULL)
                BF_URI_FREE (bas);
        return val;
}
