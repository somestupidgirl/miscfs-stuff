#include <sys/mount.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/types.h>
#include <sys/vfs_mountedfrom.h>

void vfs_mountedfrom(struct mount *vfsp, char *osname)
{
    (void) copystr(osname, vfs_statfs(vfsp)->f_mntfromname, MNAMELEN - 1, 0);
}
