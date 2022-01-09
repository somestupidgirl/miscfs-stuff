#ifndef _PSEUDOFS_DARWIN_H_INCLUDED
#define _PSEUDOFS_DARWIN_H_INCLUDED

#include <kern/locks.h>
#include <sys/mount.h>
#include <sys/proc.h>
#include <sys/systm.h>
#include <sys/vnode.h>

typedef struct node_pfs node_pfs;

struct node_pfs {
    struct mount_pfs		*pmp;
    struct vnode_pfs		*pvn;
    struct proc_pfs			*pp;
};


/*
 * ==== sysctl ====
 */

/* 
 * FIXME: Handler is MP safe.
 * From FreeBSD's sys/sysctl.h
 */
#define CTLFLAG_MPSAFE ENOTSUP


/*
 * ==== mutex ====
 */

struct mtx {
    lck_mtx_t	*mtx_lock;
    lck_grp_t	*mtx_grp;
};

#define mtx_init(mp, name, type, opts)  do {                        \
    (mp)->mtx_grp = lck_grp_alloc_init(name, LCK_GRP_ATTR_NULL);    \
        (mp)->mtx_lock = lck_mtx_alloc_init((mp)->mtx_grp,          \
         LCK_ATTR_NULL);                                            \
} while(0)

#define mtx_destroy(mp) do {                            \
    if ((mp)->mtx_lock) {                               \
        lck_mtx_free((mp)->mtx_lock, (mp)->mtx_grp);    \
        (mp)->mtx_lock = 0;                             \
    }                                                   \
    if ((mp)->mtx_grp) {                                \
        lck_grp_free((mp)->mtx_grp);                    \
        (mp)->mtx_grp = 0;                              \
    }                                                   \
} while (0)

#define MA_OWNED        LCK_MTX_ASSERT_OWNED
#define MA_NOTOWNED     LCK_MTX_ASSERT_NOTOWNED

#define mtx_lock(mp)        lck_mtx_lock((mp)->mtx_lock)
#define mtx_unlock(mp)      lck_mtx_unlock((mp)->mtx_lock)
#define mtx_assert(mp, wht) lck_mtx_assert((mp)->mtx_lock, wht)

/*
 * FIXME: Definitions from FreeBSD's sys/lockmgr.h
 * No obvious equivalents in XNU since sys/lockmgr.h
  *is not present there at all.
 */
#define LK_CANRECURSE   ENOTSUP
#define LK_RETRY        ENOTSUP
#define LK_EXCLUSIVE    ENOTSUP
#define LK_RETRY        ENOTSUP
#define LK_INTERLOCK    ENOTSUP
#define	LK_SHARED 		ENOTSUP


/*
 * ==== mount ====
 */

typedef struct mount_pfs mount_pfs;

struct mount_pfs {
	mount_t 				mp;
	lck_mtx_t 				*mnt_mlock;
	uint32_t                mnt_flag;
	uint32_t 				mnt_kern_flag;
	qaddr_t 				mnt_data;
	struct vfsstatfs        mnt_vfsstat;
};

/* pfs_malloc() */
void* pfs_malloc(size_t);

/* 
 * FIXME: FreeBSD's MNT_ILOCK(mp) and MNT_IUNLOCK(mp) definitions from sys/mount.h
 */
#define MNT_ILOCK(mp)		lck_mtx_lock(&mp->mnt_mlock)
#define MNT_IUNLOCK(mp)		lck_mtx_unlock(&mp->mnt_mlock)

/*
 * FIXME: MNTK_NOMSYNC
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
 * Probably not the same thing but we can try it for now.
 */
#include <sys/mman.h>
#define	MNTK_NOMSYNC MS_DEACTIVATE


/*
 * ==== vnode ====
 */

typedef struct vnode_pfs vnode_pfs;

struct vnode_pfs {
	vnode_t 				 vn;
    lck_mtx_t               *v_lock;
    uint32_t                 v_flag;
    uint16_t                 v_type;
    mount_t                  v_mount;
    void                    *v_data;
};

/*
 * FIXME: Declarations from FreeBSD's sys/vnode.h that are not present in XNU.
 */
int		insmntque(struct vnode_pfs *pvp, struct mount_pfs *vmp);
void	vhold(struct vnode_pfs *);
void	vdrop(struct vnode_pfs *);
void	vput(struct vnode_pfs *pvp);
int		vget(struct vnode_pfs *pvp, int flags);

static __inline int vrefcnt(struct vnode *vp);

int	_vn_lock(struct vnode *vp, int flags, const char *file, int line);
#define vn_lock(vp, flags) _vn_lock(vp, flags, __FILE__, __LINE__)

/* 
 * From XNU's sys/vnode.h
 * Absent from MacKernelSDK due to being guarded by the XNU_KERNEL_PRIVATE definition.
 */
extern errno_t VNOP_GETATTR(vnode_t, struct vnode_attr *, vfs_context_t);

// FIXME:
#define VOP_UNLOCK(vn)
#define VOP_ISLOCKED(vp)
#define VN_IS_DOOMED(vn)
#define VREF(pvn)
#define OFF_MAX ENOTSUP
#define PID_MAX ENOTSUP
#define PRELE(proc)
#define MBF_NOWAIT ENOTSUP

/*
 * VV_ROOT
 *
 * Defined in FreeBSD's sys/vnode.h:
 *    #define VV_ROOT 0x0001    // root of its filesystem
 * 
 * Corresponds with XNU's VROOT definition in sys/vnode_internal.h:
 *    #define VROOT 0x000001 // root of its file system
 */
#ifndef VROOT
#define VROOT		0x000001        // Defined in XNU sys/vnode_internal.h
#endif
#define VV_ROOT		VROOT           // So let's define VV_ROOT as that

/*
 * FIXME: VV_PROCDEP
 *
 * Defined in FreeBSD's sys/vnode.h:
 *    #define VV_PROCDEP 0x0100 // vnode is process dependent
 *
 * XNU does not seem to have a corresponding definition in sys/vnode_internal.h
 */
#define VV_PROCD ENOTSUP

/*
 * FIXME: VI_LOCK(vp)
 *
 * XNU's vnode structure does not seem to have a corresponding member to v_interlock
 */
#if 0
#define VI_LOCK(vp) lck_mtx_lock(&(vp)->v_interlock)
#else
#define VI_LOCK(vp)
#endif

/*
 * FIXME: VN_LOCK_AREC
 *
 * Defined in FreeBSD's sys/vnode.h
 */
#if 0
#define VN_LOCK_AREC(vp) lockallowrecurse((vp)->v_vnlock)
#else
#define VN_LOCK_AREC(vp)
#endif

/*
 * FIXME: VFS_VOP_VECTOR_REGISTER
 * Defined in FreeBSD's sys/vnode.h
 */
#define VFS_VOP_VECTOR_REGISTER(vnodeops) \
	SYSINIT(vfs_vector_##vnodeops##_f, SI_SUB_VFS, SI_ORDER_ANY, \
	    vfs_vector_op_register, &vnodeops)


/*
 * ==== proc ====
 */

typedef struct proc_pfs proc_pfs;

struct proc_pfs {
    lck_mtx_t		*p_mlock;
    unsigned int	 p_flag;
};

/*
 * Current and max number of procs.
 * Defined in XNU's sys/proc_internal.h
 */
extern int nprocs, maxproc;

/*
 * NO_PID
 * Defined in XNU's sys/proc_internal.h
 */
#ifndef NO_PID
#define NO_PID 100000
#endif

/*
 * From XNU's sys/proc.h
 * Absent from MacKernelSDK due to being guarded by the XNU_KERNEL_PRIVATE definition.
 */
#ifndef P_WEXIT
#define P_WEXIT 0x00002000
#endif
#ifndef P_INMEM
#define P_INMEM 0 /* Obsolete: retained for compilation */
#endif

/*
 * Lock and unlock a process.
 * From FreeBSD's sys/proc.h 
 */
#define PROC_LOCK(p)				lck_mtx_lock(&(p)->p_mlock)
#define PROC_UNLOCK(p)				lck_mtx_unlock(&(p)->p_mlock)
#define PROC_TRYLOCK(p)				lck_mtx_try_lock(&(p)->p_mlock)
#define PROC_LOCK_ASSERT(p, type)	LCK_MTX_ASSERT(&(p)->p_mlock, (type))

/*
 * FIXME: pget() flags from FreeBSD's sys/proc.h
 */
void    faultin(struct proc *p);
int     p_cansee(struct thread *td, struct proc *pp);

/*
 * Non-zero p_lock ensures that:
 * - exit1() is not performed until p_lock reaches zero;
 * - the process' threads stack are not swapped out if they are currently
 *   not (P_INMEM).
 *
 * PHOLD() asserts that the process (except the current process) is
 * not exiting, increments p_lock and swaps threads stacks into memory,
 * if needed.
 * _PHOLD() is same as PHOLD(), it takes the process locked.
 *
 */
#define PHOLD(p) do {                                       \
    PROC_LOCK(p);                                           \
    _PHOLD(p);                                              \
    PROC_UNLOCK(p);                                         \
} while (0)
#if 0 // FIXME: XNU's proc structure does not contain member p_lock
#define _PHOLD(p) do {										\
	PROC_LOCK_ASSERT((p), MA_OWNED);						\
	KASSERT(!((p)->p_flag & P_WEXIT) || (p) == curproc,		\
		("PHOLD of exiting process %p", p));				\
	(p)->p_lock++;											\
	if (((p)->p_flag & P_INMEM) == 0)						\
		faultin((p));										\
} while (0)
#define	_PHOLD_LITE(p) do {									\
	PROC_LOCK_ASSERT((p), MA_OWNED);						\
	KASSERT(!((p)->p_flag & P_WEXIT) || (p) == curproc,		\
		("PHOLD of exiting process %p", p));				\
	(p)->p_lock++;											\
} while (0)
#define PROC_ASSERT_HELD(p) do {							\
	KASSERT((p)->p_lock > 0, ("process %p not held", p));	\
} while (0)
#else
#define _PHOLD(p)
#define	_PHOLD_LITE(p)
#define PROC_ASSERT_HELD(p)
#endif

/* 
 * FIXME: Find process by id from FreeBSD's sys/proc.h
 */
struct  proc *pfind(pid_t);

#endif /* _PSEUDOFS_DARWIN_H_INCLUDED */