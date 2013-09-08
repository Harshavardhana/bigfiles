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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "gluster.h"
#include "bigfiles.h"
#include "uri.h"

static void bigfile_gluster_gconf_free(GlusterConf *conf)
{
        free(conf->server);
        free(conf->volname);
        free(conf->file);
        free(conf->transport);
        free(conf);
}

static int gluster_volume_options(GlusterConf *conf, char *path)
{
        char *p, *q;

        if (!path) {
                return -EINVAL;
        }

        /* volume */
        p = q = path + strspn(path, "/");
        p += strcspn(p, "/");
        if (*p == '\0') {
                return -EINVAL;
        }
        conf->volname = strndup(q, p - q);

        /* image */
        p += strspn(p, "/");
        if (*p == '\0') {
                return -EINVAL;
        }
        conf->file = strdup(p);
        return 0;
}

static int bigfile_gluster_uri_parse (GlusterConf *conf, const char *filename)
{
        bURI *uri = NULL;
        int ret  = 0;

        uri = bigfile_uri_parse(filename);
        if (!uri) {
                return -EINVAL;
        }

        /* transport */
        if (!strcmp(uri->scheme, "gluster")) {
                conf->transport = strdup("tcp");
        } else if (!strcmp(uri->scheme, "gluster+tcp")) {
                conf->transport = strdup("tcp");
        } else {
                ret = -EINVAL;
                goto out;
        }

        ret = gluster_volume_options(conf, uri->path);
        if (ret < 0) {
                goto out;
        }

        conf->server = strdup(uri->server);
        conf->port = uri->port;
out:
        BF_URI_FREE(uri);
        return ret;
}

static struct glfs *bigfile_gluster_init(GlusterConf *conf, const char *filename)
{
        struct glfs *glfs = NULL;
        int ret;
        int old_errno;

        ret = bigfile_gluster_uri_parse(conf, filename);
        if (ret < 0) {
                fprintf(stderr,"Usage: file=[filesystem][+transport]://[server[:port]]/"
                        "volname/file]");
                errno = -ret;
                goto out;
        }

        glfs = glfs_new(conf->volname);
        if (!glfs) {
                goto out;
        }

        ret = glfs_set_volfile_server(glfs, conf->transport, conf->server,
                                      conf->port);
        if (ret < 0) {
                goto out;
        }

        /*
         * TODO: Use GF_LOG_ERROR instead of hard code value of 4 here when
         * GlusterFS makes GF_LOG_* macros available to libgfapi users.
         */
        ret = glfs_set_logging(glfs, "-", 4);
        if (ret < 0) {
                goto out;
        }
        ret = glfs_init(glfs);
        if (ret) {
                fprintf(stderr, "Gluster connection failed for server=%s port=%d "
                        "volume=%s file=%s transport=%s", conf->server,
                        conf->port,
                        conf->volname,
                        conf->file,
                        conf->transport);
                goto out;
        }
        return glfs;
out:
        if (glfs) {
                old_errno = errno;
                glfs_fini(glfs);
                errno = old_errno;
        }
        return NULL;
}

static int bigfile_gluster_open(BigFileGlusterState *bs, int bf_adp_flags)
{
        return 0;
}

static int bigfile_gluster_create(const char *filename)
{
        return 0;
}

static void bigfile_gluster_close(BigFileGlusterState *bs)
{
        glfs_close(bs->fd);
        glfs_fini(bs->glfs);
}

static adapter_fops fops = {
        .open             = bigfile_gluster_open,
        .close            = bigfile_gluster_close,
        .create           = bigfile_gluster_create,
};
