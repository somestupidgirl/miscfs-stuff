#include <sys/mount.h>
#include <sys/mutex.h>
#include <sys/vnode.h>

typedef struct node_pfs node_pfs;
typedef struct mount_pfs mount_pfs;
typedef struct vnode_pfs vnode_pfs;

struct node_pfs {
	struct mount_pfs		*nmp;
	struct vnode_pfs		*nvp;
};

struct mount_pfs {
	lck_mtx_t 				*mnt_mlock;
	uint32_t                mnt_flag;
	uint32_t 				mnt_kern_flag;
	qaddr_t 				mnt_data;
	struct vfsstatfs        mnt_vfsstat;
};

struct vnode_pfs {
	lck_mtx_t 				*v_lock;
	uint32_t 				v_flag;
	uint16_t 				v_type;
	mount_t 				v_mount;
	void * 					v_data;
};

#define	MNT_ILOCK(nmp)	lck_mtx_lock(&(*nmp)->mnt_mlock)
#define	MNT_IUNLOCK(nmp) lck_mtx_unlock(&(*nmp)->mnt_mlock)


// From FreeBSD's sys/vnode.h:
int 	insmntque(struct vnode *nvp, struct mount *nmp);
void 	vhold(struct vnode_pfs *);
void 	vdrop(struct vnode_pfs *);
void 	vput(struct vnode_pfs *nvp);
int		vget(struct vnode_pfs *nvp, int flags);

// From FreeBSD's sys/vnode.h:
#define VROOT 			0x000001	// Defined in XNU sys/vnode_internal.h
#define VV_ROOT			VROOT 		// Defined in XNU sys/vnode_internal.h
#define	VV_PROCDEP					// No obvious equivalent in XNU sys/vnode_internal.h
#define	VI_LOCK 					// No obvious equivalent in XNU sys/vnode_internal.h
#define VN_LOCK_AREC 				// No obvious equivalent in XNU sys/vnode_internal.h

// From FreeBSD's sys/mount.h:
// No obvious equivalent in XNU sys/mount_internal.h
#define	MNTK_NOMSYNC

// From FreeBSD's sys/lockmgr.h:
// No obvious equivalents in XNU - sys/lockmgr.h is not present
#define	LK_CANRECURSE
#define	LK_RETRY
#define	LK_EXCLUSIVE	
#define	LK_RETRY
#define LK_INTERLOCK

