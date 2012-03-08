/* fs2go - takeaway filesystem
 * Copyright (c) 2012 Robin Martinjak
 * see LICENSE for full license (BSD 2-Clause)
 */

#ifndef FS2GO_SYNC_H
#define FS2GO_SYNC_H

#include "config.h"
#include "fs2go.h"

#include <sys/stat.h>
#include <sys/types.h>


/*=============*/
/* DEFINITIONS */
/*=============*/

/*----------------------*/
/* type for mtime/ctime */
/*----------------------*/

#if HAVE_UTIMENSAT && HAVE_CLOCK_GETTIME
typedef struct timespec sync_xtime_t;
#else
typedef time_t sync_xtime_t;
#endif


/*-------------*/
/* sync object */
/*-------------*/

struct sync
{
    char *path;
    sync_xtime_t mtime;
    sync_xtime_t ctime;
};


/*-------------------------------------------*/
/* callback function type for db_load_sync() */
/*-------------------------------------------*/
typedef struct sync * (*sync_load_cb_t) (const char*, sync_xtime_t, sync_xtime_t);


/*-----------------------------*/
/* return values of get_sync() */
/*-----------------------------*/

/* file is synchronised */
#define SYNC_SYNC   0

/* file has been modified */
#define SYNC_MOD    (1 << 0)

/* file has been changed */
#define SYNC_CHG    (1 << 1)

/* file is new (sync for it never set */
#define SYNC_NEW    (1 << 2)

/* file doesn't exit on remote fs */
#define SYNC_NOT_FOUND  (1 << 3)


/*====================*/
/* EXPORTED FUNCTIONS */
/*====================*/

/* initialize/destroy needed data structures */
int sync_init(void);
int sync_destroy(void);

/* store changed sync data from "change queue" to db */
int sync_store(void);

/* free a struct sync */
void sync_free(struct sync *s);

/* mark file "path" as synchronised */
int sync_set(const char *path);

/* retrieve sync status (put lstat() info in buf) */
#define sync_get(p) sync_get_stat(p, NULL)
int sync_get_stat(const char *path, struct stat *buf);

/* rename sync entries */
int sync_rename_dir(const char *from, const char *to);
int sync_delete_dir(const char *path);
int sync_rename_file(const char *from, const char *to);
int sync_delete_file(const char *path);

#endif
