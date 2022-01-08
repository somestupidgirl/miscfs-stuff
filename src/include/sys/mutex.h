#ifndef _SYS_MUTEX_H_
#define _SYS_MUTEX_H_

#include <sys/systm.h>
#include <kern/locks.h>

struct mtx {
	lck_mtx_t	*mtx_lock;
	lck_grp_t	*mtx_grp;
};

#define mtx_init(mp, name, type, opts)  do {				\
	(mp)->mtx_grp = lck_grp_alloc_init(name, LCK_GRP_ATTR_NULL);	\
        (mp)->mtx_lock = lck_mtx_alloc_init((mp)->mtx_grp,		\
	     LCK_ATTR_NULL);						\
} while(0)

#define	mtx_destroy(mp) do {						\
	if ((mp)->mtx_lock) {						\
		lck_mtx_free((mp)->mtx_lock, (mp)->mtx_grp);		\
		(mp)->mtx_lock = 0;					\
	}								\
	if ((mp)->mtx_grp) {						\
		lck_grp_free((mp)->mtx_grp);				\
		(mp)->mtx_grp = 0;					\
	}								\
} while (0)

#define	MA_OWNED 		LCK_MTX_ASSERT_OWNED
#define	MA_NOTOWNED		LCK_MTX_ASSERT_NOTOWNED

#define mtx_lock(mp)		lck_mtx_lock((mp)->mtx_lock)
#define mtx_unlock(mp)		lck_mtx_unlock((mp)->mtx_lock)
#define	mtx_assert(mp, wht)	lck_mtx_assert((mp)->mtx_lock, wht)

#endif /* _SYS_MUTEX_H_ */
