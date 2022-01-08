#ifndef _PSEUDOFS_MOUNT_H_INCLUDED
#define _PSEUDOFS_MOUNT_H_INCLUDED

#include <sys/mount.h>
#include <sys/mutex.h>
#include <sys/vnode.h>

// Defined in sys/proc_internal.h (not the one we're using from libmasochist)
#ifndef NO_PID
#define NO_PID	100000
#endif

typedef struct node_pfs node_pfs;
typedef struct mount_pfs mount_pfs;
typedef struct vnode_pfs vnode_pfs;
typedef struct proc_pfs proc_pfs;

struct node_pfs {
	struct mount_pfs		*pmp;
	struct vnode_pfs		*pvpp;
	struct proc_pfs			*pp;
};

struct mount_pfs {
	mount_t 				mp;
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
	void 					*v_data;
};

struct proc_pfs {
	lck_mtx_t 				*p_mlock;
};

/* pfs_malloc() */
void* pfs_malloc(size_t);

/* XNU doesn't seem to have an equivalent to p_lock in struct proc.
 * Comment states: "(c) Proclock (prevent swap) count."
 * Needs further investigation.
 */
//#define	PROC_ASSERT_HELD(p) do {					\
//	KASSERT((p)->p_lock > 0, ("process %p not held", p));		\
/} while (0)
#define PROC_ASSERT_HELD(p)
#define	PROC_LOCK_ASSERT(p, type)	// lck_mtx_assert(&(p)->p_mlock, (type))	// FIXME

#define MNT_ILOCK(mp) 				//	lck_mtx_lock(&(mp)->mnt_mlock)			// FIXME
#define MNT_IUNLOCK(mp) 			//	lck_mtx_unlock(&(mp)->mnt_mlock)		// FIXME

/*
 * MNTK_NOMSYNC
 *
 * Defined in FreeBSD's sys/mount.h:
 *    #define MNTK_NOMSYNC 0x00000008 // don't do msync
 *
 * No obvious equivalent in XNU's sys/mount_internal.h
 *
 * Maybe similar to XNU's MS_DEACTIVATE msync() flag in sys/mman.h:
 *    #define MS_DEACTIVATE 0x0008 // deactivate pages, leave mapped
 * 
 * MS_DEACTIVATE is absent in FreeBSD's sys/mman.h and both have the same
 * hexadecimal number definition, just in different formatting styles.
 * Maybe they work in a similar fashion but 
 */
#include <sys/mman.h>
#define	MNTK_NOMSYNC	MS_DEACTIVATE

// From FreeBSD's sys/vnode.h:
int 	insmntque(struct vnode_pfs *pvpp, struct mount_pfs *vmp);
void 	vhold(struct vnode_pfs *);
void 	vdrop(struct vnode_pfs *);
void 	vput(struct vnode_pfs *pvpp);
int		vget(struct vnode_pfs *pvpp, int flags);

// From FreeBSD's sys/proc.h:
int		p_cansee(struct thread *td, struct proc *p);
struct	proc *pfind(pid_t);		/* Find process by id. */
#define PROC_UNLOCK(p)

/*
 * VV_ROOT
 *
 * Defined in FreeBSD's sys/vnode.h:
 *    #define VV_ROOT 0x0001	// root of its filesystem
 * 
 * Corresponds with XNU's VROOT definition in sys/vnode_internal.h:
 *    #define VROOT 0x000001 // root of its file system
 */
#ifndef VROOT
#define VROOT 			0x000001		// Defined in XNU sys/vnode_internal.h
#endif
#define VV_ROOT			VROOT 			// So let's define VV_ROOT as that

/*
 * VV_PROCDEP
 *
 * Defined in FreeBSD's sys/vnode.h:
 *    #define VV_PROCDEP 0x0100 // vnode is process dependent
 *
 * XNU does not seem to have a corresponding definition in sys/vnode_internal.h
 */
#define	VV_PROCDEP		ENOTSUP

/*
 * VI_LOCK(vp)
 *
 * Defined in FreeBSD's sys/vnode.h:
 *    #define VI_LOCK(vp) mtx_lock(&(vp)->v_interlock)
 *
 * XNU's vnode structure does not seem to have a corresponding member to v_interlock
 */
#define	VI_LOCK(vp)

/*
 * VI_LOCK(vp)
 *
 * Defined in FreeBSD's sys/vnode.h:
 *    #define VN_LOCK_AREC(vp) lockallowrecurse((vp)->v_vnlock)
 *
 * XNU does not seem to have an obvious equivalent.
 */
#define VN_LOCK_AREC 	ENOTSUP

// From FreeBSD's sys/lockmgr.h:
// No obvious equivalents in XNU - sys/lockmgr.h is not present
#define	LK_CANRECURSE	ENOTSUP
#define	LK_RETRY		ENOTSUP
#define	LK_EXCLUSIVE	ENOTSUP
#define	LK_RETRY		ENOTSUP
#define LK_INTERLOCK	ENOTSUP

#define P_WEXIT

#endif /* _PSEUDOFS_MOUNT_H_INCLUDED */