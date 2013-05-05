#ifndef __OUR_MOD__
#define __OUR_MOD__

#include <map>
#include <pthread.h>
#define MAX_TM_NUM 100
#define MAX_THD_NUM 1024
#define CACHE_WAY 8
#define MNUMBER 8899174

typedef struct tm_hist
{
	unsigned long tot;
	unsigned long abort;
	pthread_mutex_t loc;
	tm_hist()
	{
		tot=0;
		abort=0;
		pthread_mutex_init(&loc,NULL);
	}
} tm_hist_t;

typedef	struct thread_tm
{
	void *i;
	int n_t;
} thread_tm_t;

struct tm_cache
{
	unsigned int data[CACHE_WAY];
	int tag[CACHE_WAY];
	pthread_mutex_t loc;
	tm_cache()
	{
		for(int i=0;i<CACHE_WAY;i++){
			data[i]=MNUMBER;
			tag[i]=i;
		}
		pthread_mutex_init(&loc,NULL);
	}
};

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

//YZ: TODO -- need to check collision
#define my_hash(in_str) \
	char *str = in_str; \
	int h = 0; \
	while (*(str))	\
		h = h << 1 ^ *(str)++;	\
	uint32_t ret_val = (h > 0 ? h : -h) % MAX_TM_NUM;	\
	if(ret_val == 0) ret_val++;	\

//uint32_t my_hash(char *str)
//{
//	int h = 0;
//	while (*str)
//		h = h << 1 ^ *str++;
//	uint32_t ret_val = (h > 0 ? h : -h) % MAX_TM_NUM;
//
//	if(ret_val == 0) return (ret_val + 1);
//	else return ret_val;
//}

#endif
