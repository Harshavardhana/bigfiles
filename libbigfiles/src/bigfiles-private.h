#ifndef _BIGFILES_PRIVATE_H
#define _BIGFILES_PRIVATE_H

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "uri.h"

enum _bfs_boolean
{
        _bfs_false = 0,
        _bfs_true = 1
};

typedef enum _bfs_boolean bfs_boolean_t;

#define ADAPTER_GLUSTER 'gluster'
#define ADAPTER_FILE    'file'

struct _bigfile_ctx {
        char               *process_uuid;
};

typedef struct _bigfile_ctx bigfile_ctx_t;

struct bigfiles;
struct bigfiles {
        char             *adapter_scheme;
        int               adapter_port;
        char             *adapter_path;
        char             *adapter_server;
        bigfile_ctx_t    *ctx;
};

bigfile_ctx_t *bigfile_ctx_new ();
bfs_boolean_t is_valid_adapter (bURI *uri);

#endif /* !_BIGFILES_PRIVATE_H */
