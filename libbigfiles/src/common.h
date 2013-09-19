#ifndef _COMMON_H_
#define _COMMON_H_

enum _bfs_boolean
{
        _bfs_false = 0,
        _bfs_true = 1
};

typedef enum _bfs_boolean bfs_boolean_t;

/* Supported storage drivers */
#define STORAGE_DRIVER_GLUSTER "gluster"
#define STORAGE_DRIVER_FILE "file"

#endif /* _COMMON_H_ */
