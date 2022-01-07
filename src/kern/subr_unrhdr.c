#include <machine/limits.h>
#include <sys/param.h>
#include <sys/malloc.h>
#include <sys/kernel.h>
#include <sys/mutex.h>
#include <sys/systm.h>
#include <sys/types.h>
#include <sys/_unrhdr.h>

static struct mtx unitmtx;

struct unrhdr;

void
init_unrhdr(struct unrhdr *uh, u_int low, u_int high, struct mtx *mutex)
{

	KASSERT(low >= 0 && low <= high,
	    ("UNR: use error: new_unrhdr(%d, %d)", low, high));
	if (mutex != NULL)
		uh->mtx = mutex;
	else
		uh->mtx = &unitmtx;
	TAILQ_INIT(&uh->head);
	TAILQ_INIT(&uh->ppfree);
	uh->low = low;
	uh->high = high;
	uh->first = 0;
	uh->last = 1 + (high - low);
}

struct unrhdr * new_unrhdr(u_int low, u_int high, struct mtx *mutex)
{
	struct unrhdr *uh;

	uh = malloc(sizeof *uh);
	init_unrhdr(uh, low, high, mutex);
	return (uh);
}

void delete_unrhdr(struct unrhdr *uh)
{
	KASSERT(uh->busy == 0, ("unrhdr has %u allocations", uh->busy));
	KASSERT(uh->alloc == 0, ("UNR memory leak in delete_unrhdr"));
	KASSERT(TAILQ_FIRST(&uh->ppfree) == NULL,
	    ("unrhdr has postponed item for free"));
	_FREE(uh, NULL);
}
