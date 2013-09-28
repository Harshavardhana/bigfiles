/**
 * Summary: library of generic URI related routines
 * Description: library of generic URI related routines
 *              Implements RFC 2396
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
 *
 */

#include <stdio.h>

int normalize_uri_path(char *);

/**
 * normalize_uri_path:
 * @path:  pointer to the path string
 *
 * Applies the 5 normalization steps to a path string--that is, RFC 2396
 * Section 5.2, steps 6.c through 6.g.
 *
 * Normalization occurs directly on the string, no new allocation is done
 *
 * Returns 0 or an error code
 */

int
normalize_uri_path(char *path)
{
        char *cur, *out;

        if (path == NULL)
                return -1;

        /* Skip all initial "/" chars.  We want to get to the beginning of the
         * first non-empty segment.
         */
        cur = path;
        while (cur[0] == '/')
                ++cur;
        if (cur[0] == '\0')
                return 0;

        /* Keep everything we've seen so far.  */
        out = cur;

        /*
         * Analyze each segment in sequence for cases (c) and (d).
         */
        while (cur[0] != '\0') {
                /*
                 * c) All occurrences of "./", where "." is a complete path segment,
                 *    are removed from the buffer string.
                 */
                if ((cur[0] == '.') && (cur[1] == '/')) {
                        cur += 2;
                        /* '//' normalization should be done at this point too */
                        while (cur[0] == '/')
                                cur++;
                        continue;
                }

                /*
                 * d) If the buffer string ends with "." as a complete path segment,
                 *    that "." is removed.
                 */
                if ((cur[0] == '.') && (cur[1] == '\0'))
                        break;

                /* Otherwise keep the segment.  */
                while (cur[0] != '/') {
                        if (cur[0] == '\0')
                                goto done_cd;
                        (out++)[0] = (cur++)[0];
                }
                /* nomalize // */
                while ((cur[0] == '/') && (cur[1] == '/'))
                        cur++;

                (out++)[0] = (cur++)[0];
        }
done_cd:
        out[0] = '\0';

        /* Reset to the beginning of the first segment for the next sequence.  */
        cur = path;
        while (cur[0] == '/')
                ++cur;
        if (cur[0] == '\0')
                return(0);

        /*
         * Analyze each segment in sequence for cases (e) and (f).
         *
         * e) All occurrences of "<segment>/../", where <segment> is a
         *    complete path segment not equal to "..", are removed from the
         *    buffer string.  Removal of these path segments is performed
         *    iteratively, removing the leftmost matching pattern on each
         *    iteration, until no matching pattern remains.
         *
         * f) If the buffer string ends with "<segment>/..", where <segment>
         *    is a complete path segment not equal to "..", that
         *    "<segment>/.." is removed.
         *
         * To satisfy the "iterative" clause in (e), we need to collapse the
         * string every time we find something that needs to be removed.  Thus,
         * we don't need to keep two pointers into the string: we only need a
         * "current position" pointer.
         */
        while (1) {
                char *segp, *tmp;

                /* At the beginning of each iteration of this loop, "cur" points to
                 * the first character of the segment we want to examine.
                 */

                /* Find the end of the current segment.  */
                segp = cur;
                while ((segp[0] != '/') && (segp[0] != '\0'))
                        ++segp;

                /* If this is the last segment, we're done (we need at least two
                 * segments to meet the criteria for the (e) and (f) cases).
                 */
                if (segp[0] == '\0')
                        break;

                /* If the first segment is "..", or if the next segment _isn't_ "..",
                 * keep this segment and try the next one.
                 */
                ++segp;
                if (((cur[0] == '.') && (cur[1] == '.') && (segp == cur+3))
                    || ((segp[0] != '.') || (segp[1] != '.')
                        || ((segp[2] != '/') && (segp[2] != '\0')))) {
                        cur = segp;
                        continue;
                }

                /* If we get here, remove this segment and the next one and back up
                 * to the previous segment (if there is one), to implement the
                 * "iteratively" clause.  It's pretty much impossible to back up
                 * while maintaining two pointers into the buffer, so just compact
                 * the whole buffer now.
                 */

                /* If this is the end of the buffer, we're done.  */
                if (segp[2] == '\0') {
                        cur[0] = '\0';
                        break;
                }
                /* Valgrind complained, strcpy(cur, segp + 3); */
                /* string will overlap, do not use strcpy */
                tmp = cur;
                segp += 3;
                while ((*tmp++ = *segp++) != 0)
                        ;

                /* If there are no previous segments, then keep going from here.  */
                segp = cur;
                while ((segp > path) && ((--segp)[0] == '/'))
                        ;
                if (segp == path)
                        continue;

                /* "segp" is pointing to the end of a previous segment; find it's
                 * start.  We need to back up to the previous segment and start
                 * over with that to handle things like "foo/bar/../..".  If we
                 * don't do this, then on the first pass we'll remove the "bar/..",
                 * but be pointing at the second ".." so we won't realize we can also
                 * remove the "foo/..".
                 */
                cur = segp;
                while ((cur > path) && (cur[-1] != '/'))
                        --cur;
        }
        out[0] = '\0';

        /*
         * g) If the resulting buffer string still begins with one or more
         *    complete path segments of "..", then the reference is
         *    considered to be in error. Implementations may handle this
         *    error by retaining these components in the resolved path (i.e.,
         *    treating them as part of the final URI), by removing them from
         *    the resolved path (i.e., discarding relative levels above the
         *    root), or by avoiding traversal of the reference.
         *
         * We discard them from the final path.
         */
        if (path[0] == '/') {
                cur = path;
                while ((cur[0] == '/') && (cur[1] == '.') && (cur[2] == '.')
                       && ((cur[3] == '/') || (cur[3] == '\0')))
                        cur += 3;
                if (cur != path) {
                        out = path;
                        while (cur[0] != '\0')
                                (out++)[0] = (cur++)[0];
                        out[0] = 0;
                }
        }
        return 0;
}

/**
 * bigobject_uri_resolve:
 * @URI:  the URI instance found in the document
 * @base:  the base value
 *
 * Computes the final URI of the reference done by checking that
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
bigobject_uri_resolve(const char *uri, const char *base)
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
                        ref = bigobject_uri_new();
                        if (ref == NULL)
                                goto done;
                        ret = bigobject_uri_parse_into(ref, uri);
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
                bas = bigobject_uri_new();
                if (bas == NULL)
                        goto done;
                ret = bigobject_uri_parse_into(bas, base);
        }
        if (ret != 0) {
                if (ref)
                        val = bigobject_uri_to_string(ref);
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
                val = bigobject_uri_to_string(bas);
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
        res = bigobject_uri_new();
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
                val = bigobject_uri_to_string(ref);
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
        val = bigobject_uri_to_string(res);

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
 * bigobject_uri_resolve_relative:
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
bigobject_uri_resolve_relative (const char *uri, const char * base)
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
        ref = bigobject_uri_new ();
        if (ref == NULL)
                return NULL;
        /* If URI not already in "relative" form */
        if (uri[0] != '.') {
                ret = bigobject_uri_parse_into (ref, uri);
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
        bas = bigobject_uri_new ();
        if (bas == NULL)
                goto done;
        if (base[0] != '.') {
                ret = bigobject_uri_parse_into (bas, base);
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
                        val = bigobject_uri_string_escape(uptr, "/;&=+$,");
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
                        val = bigobject_uri_string_escape(uptr, "/;&=+$,");
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
        val = bigobject_uri_string_escape(vptr, "/;&=+$,");
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
