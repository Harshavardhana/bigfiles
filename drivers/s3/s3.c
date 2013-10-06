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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

#include "s3-priv.h"
#include "s3.h"


/*
  SYNOPSIS

  s3_curl_write: handles writing to I/O buffer

  PARAMETERS:
  @ptr - pointer to the incoming data
  @size - size of the individual data member
  @nmemb - total number of data members
  @stream - stream pointer to allocated io buffer

  RETURN VALUES:
  N : Total number of bytes processed
*/

static size_t s3_curl_write (void *ptr, size_t size, size_t nmemb, void *stream)
{
        return s3_iobuf_add (stream, ptr, (nmemb * size));
}

/*
  SYNOPSIS

  s3_curl_read: Handles reading through stream per line

  PARAMETERS:
  @ptr - pointer to the incoming data
  @size - size of the individual data member
  @nmemb - total number of data members
  @stream - stream pointer to allocated io buffer

  RETURN VALUES:
  N : Total number of bytes sent
*/

static size_t s3_curl_read (void *ptr, size_t size, size_t nmemb, void *stream)
{
        return s3_iobuf_getline (stream, ptr, (nmemb * size));
}

/*
  SYNOPSIS

  process_header: Process incoming header

  PARAMETERS:
  @ptr - pointer to the incoming data
  @size - size of the individual data member
  @nmemb - total number of data members
  @stream - stream pointer to allocated io buffer

  RETURN VALUES:
  N : Total number of bytes sent
*/

static size_t process_header (void *ptr, size_t size, size_t nmemb,
                              void *stream)
{
        iobuf_t *buf = NULL;

        if (!stream)
                return -1;

        if (!strncmp (ptr, "HTTP/1.1", 8)) {
                buf->reply = strdup ((char *)ptr + 9);
                _chomp (buf->reply);
                buf->code = atoi ((char *)ptr + 9);
        } else if (!strncmp (ptr, "Etag: ", 6)) {
                buf->etag = strdup ((char *)ptr + 6);
                _chomp (buf->etag);
        } else if (!strncmp (ptr, "Last-Modified: ", 14)) {
                buf->lastmod = strdup ((char *)ptr + 15);
                _chomp (buf->lastmod);
        } else if (!strncmp (ptr, "Content-Length: ", 15)) {
                buf->contentlen = atoi ((char *)ptr + 16);
        }

        return (nmemb * size);
}

static
char *__s3_sign (const char *str, const char *awskey)
{
        uchar_t   *MD;
        char      *base64 = NULL;

        MD = HMAC(EVP_sha1(), awskey, strlen(awskey), (uchar_t *) str,
                  strlen(str), NULL, NULL);

        base64 = _base64_encode (MD);

        return base64;
}

/*
  Date in HTTP format - RFC2616
  --------------------------------------------------------
  HTTP-date    = rfc1123-date | rfc850-date | asctime-date
  rfc1123-date = wkday "," SP date1 SP time SP "GMT"
  rfc850-date  = weekday "," SP date2 SP time SP "GMT"
  asctime-date = wkday SP date3 SP time SP 4DIGIT
  date1        = 2DIGIT SP month SP 4DIGIT
                 ; day month year (e.g., 02 Jun 1982)
  date2        = 2DIGIT "-" month "-" 2DIGIT
                 ; day-month-year (e.g., 02-Jun-82)
  date3        = month SP ( 2DIGIT | ( SP 1DIGIT ))
                 ; month day (e.g., Jun  2)
  time         = 2DIGIT ":" 2DIGIT ":" 2DIGIT
                 ; 00:00:00 - 23:59:59
  wkday        = "Mon" | "Tue" | "Wed"
               | "Thu" | "Fri" | "Sat" | "Sun"
  weekday      = "Monday" | "Tuesday" | "Wednesday"
               | "Thursday" | "Friday" | "Saturday" | "Sunday"
  month        = "Jan" | "Feb" | "Mar" | "Apr"
               | "May" | "Jun" | "Jul" | "Aug"
               | "Sep" | "Oct" | "Nov" | "Dec"
*/

static
int32_t get_http_date (char **date)
{
        char tmp_date[256];
        time_t t = {0,};
        const struct tm *utc = NULL;

        t = time (NULL);

        utc = gmtime (&t);

        if (!utc)
                return -1;

        strftime (tmp_date, sizeof(tmp_date), "%a, %d %b %Y %H:%M:%S %Z", utc);

        *date = tmp_date;

        return 0;
}

static
int32_t s3_set_signature_request(char *resource, const char *date,
                                 const char *method, const char *object,
                                 const struct s3_conf *s3conf,
                                 char **signature)
{
        char  sign_hdr [2048];
        char  acl [32];
        char  rrs [64];

        if (!s3conf)
                return 0;

        if (object)
                snprintf (resource, strlen(resource), "%s", object);

        if (s3conf->acls)
                snprintf(acl, sizeof(acl), "x-amz-acl:%s\n", s3conf->acls);
        else
                acl[0] = 0;

        if (s3conf->use_rrs)
                strncpy(rrs, "x-amz-storage-class:REDUCED_REDUNDANCY\n",
                        sizeof(rrs));
        else
                rrs[0] = 0;

        snprintf (sign_hdr, sizeof(sign_hdr), "%s\n\n%s\n%s\n%s%s/%s",
                  method, s3conf->mime_type ? s3conf->mime_type : "",
                  date, acl, rrs, resource);

        *signature = __s3_sign (sign_hdr, s3conf->awskey);

        return 0;
}


static
int32_t s3_do_put (iobuf_t *iob, const char *signature,
                   const char *date, const char *resource,
                   struct s3_conf *s3conf)
{
        char              request[1024];
        CURL              *ch = NULL;
        struct curl_slist *slist = NULL;
        int32_t           ret = -1;

        if ((!iob) || (!signature) || (!date) || (!resource) || (!s3conf))
                goto out;

        ch = curl_easy_init();

        if (s3conf->mime_type) {
                snprintf (request, sizeof(request), "Content-Type: %s",
                          s3conf->mime_type);
                slist = curl_slist_append(slist, request);
        }

        if (s3conf->acls) {
                snprintf (request, sizeof(request), "x-amz-acl: %s",
                          s3conf->acls);
                slist = curl_slist_append(slist, request);
        }

        if (s3conf->use_rrs) {
                strncpy (request, "x-amz-storage-class: REDUCED_REDUNDANCY",
                         sizeof(request));
                slist = curl_slist_append(slist, request);
        }

        if (date) {
                snprintf (request, sizeof(request), "Date: %s", date);
                slist = curl_slist_append(slist, request);
        }

        if ((s3conf->awskey_id && signature)) {
                snprintf (request, sizeof(request), "Authorization: AWS %s:%s",
                          s3conf->awskey_id, signature);
                slist = curl_slist_append(slist, request);
        }

        if (s3conf->s3host && resource)
                snprintf (request, sizeof(request), "http://%s/%s",
                          s3conf->s3host, resource);

        curl_easy_setopt (ch, CURLOPT_HTTPHEADER, slist);
        curl_easy_setopt (ch, CURLOPT_URL, request);
        curl_easy_setopt (ch, CURLOPT_READDATA, iob);
        curl_easy_setopt (ch, CURLOPT_WRITEFUNCTION, s3_curl_write);
        curl_easy_setopt (ch, CURLOPT_READFUNCTION, s3_curl_read);
        curl_easy_setopt (ch, CURLOPT_HEADERFUNCTION, process_header);
        curl_easy_setopt (ch, CURLOPT_HEADERDATA, iob);
        curl_easy_setopt (ch, CURLOPT_VERBOSE, 0);
        curl_easy_setopt (ch, CURLOPT_UPLOAD, 1);
        curl_easy_setopt (ch, CURLOPT_INFILESIZE, iob->len);
        curl_easy_setopt (ch, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt (ch, CURLOPT_ERRORBUFFER, 1);

        curl_easy_perform (ch);
        curl_slist_free_all (slist);
        curl_easy_cleanup (ch);

        ret = 0;
out:
        return ret;
}


static
int32_t s3_do_get (iobuf_t *iob, const char *signature,
                   const char *date, const char *resource,
                   struct s3_conf *s3conf)
{
        char              request[1024];
        CURL              *ch    = NULL;
        struct curl_slist *slist = NULL;
        int32_t           ret = -1;

        if ((!iob) || (!signature) || (!date) || (!resource) || (!s3conf))
                goto out;

        ch = curl_easy_init ();

        snprintf (request, sizeof(request), "Date: %s", date);
        slist = curl_slist_append (slist, request);

        snprintf (request, sizeof(request), "Authorization: AWS %s:%s",
                  s3conf->awskey_id, signature);
        slist = curl_slist_append(slist, request);

        snprintf (request, sizeof(request), "http://%s/%s",
                  s3conf->s3host, resource);

        curl_easy_setopt (ch, CURLOPT_HTTPHEADER, slist);
        curl_easy_setopt (ch, CURLOPT_URL, request);
        curl_easy_setopt (ch, CURLOPT_WRITEFUNCTION, s3_curl_write);
        curl_easy_setopt (ch, CURLOPT_WRITEDATA, iob);
        curl_easy_setopt (ch, CURLOPT_HEADERFUNCTION, process_header);
        curl_easy_setopt (ch, CURLOPT_HEADERDATA, iob);
        curl_easy_setopt (ch, CURLOPT_ERRORBUFFER, 1);

        curl_easy_perform(ch);
        curl_slist_free_all(slist);
        curl_easy_cleanup(ch);

        ret = 0;
out:
        return ret;

}

static
int32_t s3_do_delete (iobuf_t *iob, const char *signature,
                      const char *date, const char *resource,
                      struct s3_conf *s3conf)
{
        char              request[1024];
        CURL              *ch = NULL;
        struct curl_slist *slist = NULL;
        int32_t           ret = -1;

        if ((!iob) || (!signature) || (!date) || (!resource) || (!s3conf))
                goto out;

        ch = curl_easy_init ();

        snprintf (request, sizeof(request), "Date: %s", date);
        slist = curl_slist_append(slist, request);

        snprintf (request, sizeof(request), "Authorization: AWS %s:%s",
                  s3conf->awskey_id, signature);
        slist = curl_slist_append(slist, request);

        snprintf (request, sizeof(request), "http://%s/%s",
                  s3conf->s3host, resource);

        curl_easy_setopt (ch, CURLOPT_CUSTOMREQUEST, "DELETE");
        curl_easy_setopt (ch, CURLOPT_HTTPHEADER, slist);
        curl_easy_setopt (ch, CURLOPT_URL, request);
        curl_easy_setopt (ch, CURLOPT_HEADERFUNCTION, process_header);
        curl_easy_setopt (ch, CURLOPT_HEADERDATA, iob);

        curl_easy_perform(ch);
        curl_slist_free_all(slist);
        curl_easy_cleanup(ch);

        ret = 0;
out:
        return ret;

}


int32_t s3_put (iobuf_t *iob, struct s3_conf *s3conf, const char*object)
{
        char  resource [1024] = "";
        char *date            = NULL;
        char *signature       = NULL;
        int32_t ret           = -1;

        if ((!s3conf) || (!object) || (!iob))
                goto out;

        get_http_date (&date);

        s3_set_signature_request (resource, date, "PUT",
                                  object, s3conf,
                                  &signature);

        ret = s3_do_put (iob, signature, date, resource, s3conf);

        if (signature)
                free (signature);
out:
        return ret;
}

int s3_get (iobuf_t *iob, struct s3_conf *s3conf, const char *object)
{
        char  resource [1024] = "";
        char  *date           = NULL;
        char  *signature      = NULL;
        int32_t ret           = -1;

        if ((!s3conf) || (!object) || (!iob))
                goto out;

        get_http_date (&date);

        s3_set_signature_request (resource, date, "GET",
                                  object, s3conf,
                                  &signature);

        ret = s3_do_get (iob, signature, date, resource, s3conf);

        if (signature)
                free (signature);
out:
        return ret;
}

int s3_delete (iobuf_t *iob, struct s3_conf *s3conf, const char *object)
{
        char  resource [1024] = "";
        char  *date           = NULL;
        char  *signature      = NULL;
        int32_t ret           = -1;

        if ((!s3conf) || (!object) || (!iob))
                goto out;

        get_http_date (&date);

        s3_set_signature_request (resource, date, "GET",
                                  object, s3conf,
                                  &signature);

        ret = s3_do_delete (iob, signature, date, resource, s3conf);

        if (signature)
                free (signature);
out:
        return ret;

}


struct s3_conf *s3_init (const char *account_id,
                         const char *awskey_id,
                         const char *awskey,
                         const char *s3host,
                         const char *bktname)
{
        struct s3_conf *s3conf = NULL;

        if ((!account_id) || (!awskey_id) || (!awskey) ||
            (!s3host) || (!bktname)) {
                errno = -EINVAL;
                return NULL;
        }

        s3conf = calloc (1, sizeof(*s3conf));

        if (!s3conf) {
                errno = -ENOMEM;
                return NULL;
        }

        s3conf->account_id = strdup (account_id);
        s3conf->awskey_id = strdup (awskey_id);
        s3conf->awskey = strdup (awskey);
        s3conf->s3host = strdup (s3host);
        s3conf->bucket_name = strdup (bktname);
        s3conf->use_rrs = 0;
        s3conf->mime_type = NULL;
        s3conf->acls = NULL;

        curl_global_init (CURL_GLOBAL_ALL);

        return s3conf;
}

void s3_fini (struct s3_conf *s3conf)
{
        if (s3conf) {
                if (s3conf->account_id)
                        free (s3conf->account_id);
                else if (s3conf->awskey_id)
                        free (s3conf->awskey_id);
                else if (s3conf->awskey)
                        free (s3conf->awskey);
                else if (s3conf->s3host)
                        free (s3conf->s3host);
                else if (s3conf->bucket_name)
                        free (s3conf->bucket_name);
                else if (s3conf->mime_type)
                        free (s3conf->mime_type);
                else if (s3conf->acls)
                        free (s3conf->acls);
                free (s3conf);
        }
}
