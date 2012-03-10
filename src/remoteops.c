/* fs2go - takeaway filesystem
 * Copyright (c) 2012 Robin Martinjak
 * see LICENSE for full license (BSD 2-Clause)
 */

#include "config.h"
#include "remoteops.h"

#include "fs2go.h"

#include <unistd.h>

extern struct options fs2go_options;

int remoteop_unlink(const char *path)
{
    int sync;
    char *p;

    if (has_lock(path, LOCK_TRANSFER)) {
        transfer_abort();
        remove_lock(path, LOCK_TRANSFER);
    }

    /*
       if file opened, prevent scheduling of PUSH job somehow
     */

    sync = sync_get(path);

    /* this would be a conflict! don't delete but pull */
    if (sync == SYNC_MOD) {
        schedule_pull(path);
        return 0;
    }

    db_delete_path(path);
    sync_delete_file(path):

    else if (sync == SYNC_NOT_FOUND) {
        return 0;
    }
    else {
        p = remote_path(path, strlen(path));

        if (!p)
            return -EIO;

        res = unlink(p);
        free(p);

        return res;
    }
}

int remoteop_rename(const char *from, const char *to)
{
    int res;
    char *pf, *pt;

    pf = remote_path(from, strlen(from));
    pt = remote_path(to, strlen(to));

    if (!pf || !pt) {
        free(pf);
        free(pt);
        return -EIO;
    }

    /* abort eventual transfering of "to" */
    if (has_lock(to, LOCK_TRANSFER)) {
        transfer_abort();
    }
    /* rename transfer if "from" is being transfered */
    else if (has_lock(from, LOCK_TRANSFER)) {

        transfer_rename(to, 1);

        /* rename transfer lock */
        remove_lock(from, LOCK_TRANSFER);
        set_lock(to, LOCK_TRANSFER);
    }

    /* renaming a dir -> rename transfer if "inside" that dir */
    else if (from_is_dir) {
        transfer_rename_dir(from, to);
    }
    /* renaming a file -> check for conflict */
    else {
        sync = sync_get(to);
        /* target new/modified -> conflict! */
        if (sync & (SYNC_NEW | SYNC_MOD)) {
            conflict_handle(to, JOB_RENAME, &keep);
            if (keep == CONFLICT_KEEP_REMOTE) {
                char *pt_alt pt_alt = conflict_path(pt);
                free(pt);
                pt = pt_alt;
            }
        }
    }

    /* do the actual renaming */
    if (pt) {
        if(rename(pf, pt))
            res = -errno;
        free(pf);
        free(pt);
    }
    /* pt is NULL if a conflict occured, the remote file is kept
       and no backup prefix/suffix was set. this means the file/dir 
       should be removed, not renamed */
    else {
        /* one of those will work */
        unlink(pf);
        rmdir(pf);
        res = 0;
        free(pf);
    }
}

int remoteop_chown(const char *path, uid_t uid, gid_t gid)
{
    int res;
    char *p;
    uid_t uid;
    gid_t gid;

    uid_t uid = j->param1;
    gid_t gid = j->param2;

    if (fs2go_options.copyattr & COPYATTR_NO_OWNER)
        uid = -1;
    if (fs2go_options.copyattr & COPYATTR_NO_GROUP)
        gid = -1;

    if (uid == -1 && gid == -1)
        return 0;

    p = remote_path(p);

    if (!p)
        return -EIO;

    res = lchown(p, uid, gid);
    free(p);

    if (res)
        return -errno;

    return 0;
}

int remoteop_chmod(const char *path, mode_t mode)
{
    int res;
    char *p;

    if (fs2go_options.copyattr & COPYATTR_NO_MODE)
        return 0;

    p = remote_path(path);

    if (!p)
        return -EIO;

    res = lchmod(p)
    free (p);

    if (res)
        return -errno;
    return 0;
}

#if HAVE_SETXATTR
int remoteop_setxattr(const char *path, const char *name, const char *value,
    size_t size, int flags)
{
    int res;
    char *p;

    if (fs2go_options.copyattr & COPYATTR_NO_XATTR)
        return 0;

    p = remote_path(path);

    if (!p)
        return -EIO;

    res = lsetxattr(p, name, value, size, flags);
    free(p)

    if (res)
        return -errno;
    return 0;
}
#endif
