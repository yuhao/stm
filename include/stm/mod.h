#ifndef __OUR_MOD__
#define __OUR_MOD__

#include <map>

typedef struct tm_hist
{
	unsigned long tot;
	unsigned long abort;
} tm_hist_t;

typedef	struct thread_tm
{
	void *i;
	int n_t;
} thread_tm_t;

typedef std::map<char *, tm_hist_t> tm_hist_map;

#if defined(STM_WS_BYTELOG)
#   define MY_STM_READ_SIG(tx, addr, mask, abort)       TxThread* tx, void** addr, uintptr_t mask, bool *abort
#   define MY_STM_WRITE_SIG(tx, addr, val, mask, abort) TxThread* tx, void** addr, void* val, uintptr_t mask, bool *abort
#else
#   define MY_STM_READ_SIG(tx, addr, mask, abort)       TxThread* tx, void** addr, bool *abort
#   define MY_STM_WRITE_SIG(tx, addr, val, mask, abort) TxThread* tx, void** addr, void* val, bool *abort
#endif

#if defined(STM_PROTECT_STACK)
#   define MY_STM_COMMIT_SIG(tx, stack, abort)  TxThread* tx, void** stack, bool *abort
#else
#   define MY_STM_COMMIT_SIG(tx, stack, abort)  TxThread* tx, bool *abort
#endif

#endif
