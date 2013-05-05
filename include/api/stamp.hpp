/**
 *  Copyright (C) 2011
 *  University of Rochester Department of Computer Science
 *    and
 *  Lehigh University Department of Computer Science and Engineering
 *
 * License: Modified BSD
 *          Please see the file LICENSE.RSTM for licensing information
 */

//YZ
#include <map>
#include <stdlib.h>
#include <string.h>
#include <stm/mod.h>
#include <pthread.h>
#include <stm/txthread.hpp>
#include <time.h>
#include <sys/time.h>

#ifndef API_STAMP_HPP__
#define API_STAMP_HPP__

#include <setjmp.h>
#include <cstdlib>
#include <cassert>
#include <api/library.hpp>

/**
 *  We are gradually moving to a state where STAMP will be in pure C++.
 *  Clearly we are not there yet, but currently we have STAMP set to build with
 *  g++ instead of gcc.
 *
 *  That being the case, we're going to avoid some cruft by mapping MACROs
 *  directly to c++ functions, instead of hiding code within an 'extern C'
 *  block with C-style wrapper functions.
 */

#define STM_THREAD_T             stm::TxThread
#define STM_SELF                 tm_descriptor
#define STM_STARTUP(numThread)   tm_main_startup()
#define STM_SHUTDOWN()           stm::sys_shutdown()
#define STM_NEW_THREAD()         0
#define STM_INIT_THREAD(t, id)   tm_start(&t, thread_getId())
#define STM_FREE_THREAD(t)
#define STM_RESTART()            stm::restart()

#define STM_LOCAL_WRITE_I(var, val) ({var = val; var;})
#define STM_LOCAL_WRITE_L(var, val) ({var = val; var;})
#define STM_LOCAL_WRITE_F(var, val) ({var = val; var;})
#define STM_LOCAL_WRITE_P(var, val) ({var = val; var;})

/**
 *  Special alloc for STAMP, used to detect nontransactional mallocs from
 *  within transactions
 */
inline void* tx_safe_non_tx_alloc(size_t size)
{
    return malloc(size);
}

/**
 *  Special free for STAMP, used to detect nontransactional frees from within
 *  transactions
 */
inline void tx_safe_non_tx_free(void * ptr)
{
    free(ptr);
}

/**
 *  The begin and commit instrumentation are straightforward
 */
//YZ
//tm_hist[tx->id][tm].abort++;
//tm_hist[tx->id][tm].tot++;
//gettimeofday(&tx->cache_start, NULL);	
//gettimeofday(&tx->cache_end, NULL);
//tx->cache_overhead += (((double)(tx->cache_end).tv_sec * 1000000.0 + (double)(tx->cache_end).tv_usec) - ((double)(tx->cache_start).tv_sec * 1000000.0 + (double)(tx->cache_start).tv_usec)) / 1000000.0;

#define STM_BEGIN_WR()                                                  \
{                                                                   \
	stm::TxThread* tx = (stm::TxThread*)stm::Self;          \
	static tm_hist_map tm_hist[MAX_THD_NUM]; \
	static unsigned long active_tm[MAX_TM_NUM];	\
    static struct tm_cache ccache[MAX_TM_NUM];\
	char tm[100]; \
	sprintf(tm, "%s#%d", __func__, __LINE__); \
	strcpy(tx->current_tm, tm);	\
	my_hash(tm);	\
	uint32_t cur_tm = ret_val;	\
	jmp_buf _jmpbuf;                                        \
	static pthread_mutex_t lock1, lock2;	\
	uint32_t abort_flags = setjmp(_jmpbuf);                 \
	tm_hist[tx->id][tm].tot++;	\
	if(abort_flags)	\
	{	\
		tm_hist[tx->id][tm].abort++;\
		pthread_mutex_lock(&((ccache[cur_tm]).loc));			\
		int way=-1;\
		for(int i=0;i<CACHE_WAY;i++){\
				if((ccache[cur_tm]).data[i]==abort_flags){\
					way=i;\
					break;\
				}\
		}\
		if(way==-1){\
			for(int i=0;i<CACHE_WAY;i++){\
				if((ccache[cur_tm]).tag[i]==CACHE_WAY-1){\
					way=i;\
					break;\
				}\
			}\
			(ccache[cur_tm]).data[way]=abort_flags;\
		}\
		for(int i=0;i<CACHE_WAY;i++){\
				if((ccache[cur_tm]).tag[i]<way){\
					(ccache[cur_tm]).tag[i]++;\
				}\
		}\
		(ccache[cur_tm]).tag[way]=0;\
		pthread_mutex_unlock(&((ccache[cur_tm]).loc));			\
		pthread_mutex_lock(&lock2);			\
		active_tm[cur_tm]--;	\
		pthread_mutex_unlock(&lock2);			\
	}	\
	pthread_mutex_lock(&lock1);			\
	if(10*tm_hist[tx->id][tm].abort>tm_hist[tx->id][tm].tot)\
	{\
		while(1) \
		{	\
			bool can_enter = true;	\
			for(int i = 0;i < CACHE_WAY;i++)	\
			{	\
				pthread_mutex_lock(&((ccache[cur_tm]).loc));			\
				unsigned long abort_local = (ccache[cur_tm]).data[i];	\
				pthread_mutex_unlock(&((ccache[cur_tm]).loc));			\
				pthread_mutex_lock(&lock2);			\
				unsigned long num_active = active_tm[abort_local];	\
				pthread_mutex_unlock(&lock2);			\
				if((abort_local!=MNUMBER)&&(abort_local && num_active ))	\
				{	\
					can_enter = false;	\
					break;	\
				}	\
			}	\
			if(can_enter) break;	\
		}	\
	}\
	pthread_mutex_lock(&lock2);			\
	active_tm[cur_tm]++;	\
	pthread_mutex_unlock(&lock2);			\
	pthread_mutex_unlock(&lock1);			\
	stm::begin(tx, &_jmpbuf, abort_flags);        	\
	CFENCE;                                                 \
{

#define HASH_STM_BEGIN_WR()                                                  \
{                                                                   \
	stm::TxThread* tx = (stm::TxThread*)stm::Self;          \
	static tm_hist_map tm_hist[MAX_THD_NUM]; \
	static unsigned long tm_abort[MAX_TM_NUM][MAX_TM_NUM];	\
	static unsigned long active_tm[MAX_TM_NUM];	\
	char tm[100]; \
	sprintf(tm, "%s#%d", __func__, __LINE__); \
	strcpy(tx->current_tm, tm);	\
	my_hash(tm);	\
	uint32_t cur_tm = ret_val;	\
	jmp_buf _jmpbuf;                                        \
	static pthread_mutex_t lock1, lock2, lock3;	\
	uint32_t abort_flags = setjmp(_jmpbuf);                 \
	if(abort_flags)	\
	{	\
		pthread_mutex_lock(&lock3);			\
		tm_abort[cur_tm][abort_flags]++;	\
		pthread_mutex_unlock(&lock3);			\
		pthread_mutex_lock(&lock2);			\
		active_tm[cur_tm]--;	\
		pthread_mutex_unlock(&lock2);			\
	}	\
	pthread_mutex_lock(&lock1);			\
	gettimeofday(&tx->start, NULL);	\
	while(1) \
	{	\
		bool can_enter = true;	\
		for(int i = 0;i < MAX_TM_NUM;i++)	\
		{	\
			pthread_mutex_lock(&lock3);			\
			unsigned long abort_local = tm_abort[cur_tm][i];	\
			pthread_mutex_unlock(&lock3);			\
			pthread_mutex_lock(&lock2);			\
			unsigned long num_active = active_tm[i];	\
			pthread_mutex_unlock(&lock2);			\
			if(abort_local && num_active )	\
			{	\
				can_enter = false;	\
				break;	\
			}	\
		}	\
		if(can_enter) break;	\
	}	\
	gettimeofday(&tx->end, NULL);	\
	tx->tot_overhead += (((double)(tx->end).tv_sec * 1000000.0 + (double)(tx->end).tv_usec) - ((double)(tx->start).tv_sec * 1000000.0 + (double)(tx->start).tv_usec)) / 1000000.0;	\
	pthread_mutex_lock(&lock2);			\
	active_tm[cur_tm]++;	\
	pthread_mutex_unlock(&lock2);			\
	pthread_mutex_unlock(&lock1);			\
	stm::begin(tx, &_jmpbuf, abort_flags);        	\
	CFENCE;                                                 \
{

//gettimeofday(&tx->wait_start, NULL);
//gettimeofday(&tx->wait_end, NULL);
//tx->wait_overhead += (((double)(tx->wait_end).tv_sec * 1000000.0 + (double)(tx->wait_end).tv_usec) - ((double)(tx->wait_start).tv_sec * 1000000.0 + (double)(tx->wait_start).tv_usec)) / 1000000.0;

#define LRC_STM_BEGIN_WR()                                                  \
{                                                                   \
	stm::TxThread* tx = (stm::TxThread*)stm::Self;          \
	static tm_hist_map tm_hist[MAX_THD_NUM]; \
	static unsigned long active_tm[MAX_TM_NUM];	\
    static struct tm_cache ccache[MAX_TM_NUM];\
	char tm[100]; \
	sprintf(tm, "%s#%d", __func__, __LINE__); \
	strcpy(tx->current_tm, tm);	\
	my_hash(tm);	\
	uint32_t cur_tm = ret_val;	\
	jmp_buf _jmpbuf;                                        \
	static pthread_mutex_t lock1, lock2;	\
	uint32_t abort_flags = setjmp(_jmpbuf);                 \
	if(abort_flags)	\
	{	\
		pthread_mutex_lock(&((ccache[cur_tm]).loc));			\
		gettimeofday(&tx->cache_start, NULL);	\
		int way=-1;\
		for(int i=0;i<CACHE_WAY;i++){\
				if((ccache[cur_tm]).data[i]==abort_flags){\
					way=i;\
					break;\
				}\
		}\
		if(way==-1){\
			for(int i=0;i<CACHE_WAY;i++){\
				if((ccache[cur_tm]).tag[i]==CACHE_WAY-1){\
					way=i;\
					break;\
				}\
			}\
			(ccache[cur_tm]).data[way]=abort_flags;\
		}\
		for(int i=0;i<CACHE_WAY;i++){\
				if((ccache[cur_tm]).tag[i]<way){\
					(ccache[cur_tm]).tag[i]++;\
				}\
		}\
		(ccache[cur_tm]).tag[way]=0;\
		gettimeofday(&tx->cache_end, NULL);	\
		tx->cache_overhead += (((double)(tx->cache_end).tv_sec * 1000000.0 + (double)(tx->cache_end).tv_usec) - ((double)(tx->cache_start).tv_sec * 1000000.0 + (double)(tx->cache_start).tv_usec)) / 1000000.0;	\
		pthread_mutex_unlock(&((ccache[cur_tm]).loc));			\
		pthread_mutex_lock(&lock2);			\
		active_tm[cur_tm]--;	\
		pthread_mutex_unlock(&lock2);			\
	}	\
	pthread_mutex_lock(&lock1);			\
	gettimeofday(&tx->wait_start, NULL);	\
	while(1) \
	{	\
		bool can_enter = true;	\
		for(int i = 0;i < CACHE_WAY;i++)	\
		{	\
			pthread_mutex_lock(&((ccache[cur_tm]).loc));			\
			unsigned long abort_local = (ccache[cur_tm]).data[i];	\
			pthread_mutex_unlock(&((ccache[cur_tm]).loc));			\
			pthread_mutex_lock(&lock2);			\
			unsigned long num_active = active_tm[abort_local];	\
			pthread_mutex_unlock(&lock2);			\
			if((abort_local!=MNUMBER)&&(abort_local && num_active ))	\
			{	\
				can_enter = false;	\
				break;	\
			}	\
		}	\
		if(can_enter) break;	\
	}	\
	gettimeofday(&tx->wait_end, NULL);	\
	tx->wait_overhead += (((double)(tx->wait_end).tv_sec * 1000000.0 + (double)(tx->wait_end).tv_usec) - ((double)(tx->wait_start).tv_sec * 1000000.0 + (double)(tx->wait_start).tv_usec)) / 1000000.0;	\
	pthread_mutex_lock(&lock2);			\
	active_tm[cur_tm]++;	\
	pthread_mutex_unlock(&lock2);			\
	pthread_mutex_unlock(&lock1);			\
	stm::begin(tx, &_jmpbuf, abort_flags);        	\
	CFENCE;                                                 \
{

#define STM_END()                                   \
}                                               \
	stm::commit(tx);                            \
	pthread_mutex_lock(&lock2);			\
	active_tm[cur_tm]--;	\
	pthread_mutex_unlock(&lock2);			\
}

#define ORI_STM_BEGIN_WR()                                                  \
    {                                                                   \
    jmp_buf jmpbuf_;                                                    \
    uint32_t abort_flags = setjmp(jmpbuf_);                             \
    begin(static_cast<stm::TxThread*>(STM_SELF), &jmpbuf_, abort_flags);\
    CFENCE;                                                             \
    {

#define ORI_STM_END()                                   \
    }                                               \
    commit(static_cast<stm::TxThread*>(STM_SELF));  \
    }

/*** read-only begin == read/write begin */
#define STM_BEGIN_RD() STM_BEGIN_WR()

/**
 *  tm_main_startup()
 *
 *  call before any threads try to run transactions, in order to ensure
 *  that the TM library is properly configured.
 *
 *  multiple calls are safe, since the library protects itself
 */
inline void tm_main_startup()
{
    // start the STM runtime
    stm::sys_init(NULL);

    // create a descriptor for this thread
    stm::thread_init();
}

/**
 *  tm_start(desc, id)
 *
 *  STAMP uses this during its main processing loop, once all of the
 *  threads have been created and are sitting at a barrier waiting to
 *  start. We expect it to be only called once per thread, but it is ok if
 *  it is called more than once.
 *
 *  The thread that called tm_main_startup(...) /can/, but does not have
 *  to, call this routine.
 */
inline void tm_start(stm::TxThread** desc, int id)
{
    stm::thread_init();
    // The desc parameter is an "out" parameter, so return its address
    *desc = stm::Self;
}

#endif // API_STAMP_HPP__
