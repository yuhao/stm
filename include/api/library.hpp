/**
 *  Copyright (C) 2011
 *  University of Rochester Department of Computer Science
 *    and
 *  Lehigh University Department of Computer Science and Engineering
 *
 * License: Modified BSD
 *          Please see the file LICENSE.RSTM for licensing information
 */

/**
 *  This file presents a simple library API for using RSTM without compiler
 *  support.  The API consists of the following:
 *
 *  TM_ALLOC            : Allocate memory inside a transaction
 *  TM_FREE             : Deallocate memory inside a transaction
 *  TM_SYS_INIT         : Initialize the STM library
 *  TM_SYS_SHUTDOWN     : Shut down the STM library
 *  TM_THREAD_INIT      : Initialize a thread before using TM
 *  TM_THREAD_SHUTDOWN  : Shut down a thread
 *  TM_SET_POLICY(P)    : Change the STM algorithm on the fly
 *  TM_BECOME_IRREVOC() : Become irrevocable or abort
 *  TM_READ(var)        : Read from shared memory from a txn
 *  TM_WRITE(var, val)  : Write to shared memory from a txn
 *  TM_BEGIN(type)      : Start a transaction... use 'atomic' as type
 *  TM_END              : End a transaction
 *
 *  Custom Features:
 *
 *  stm::restart()                : Self-abort and immediately retry a txn
 *  TM_BEGIN_FAST_INITIALIZATION  : For fast initialization
 *  TM_END_FAST_INITIALIZATION    : For fast initialization
 *  TM_GET_ALGNAME()              : Get the current algorithm name
 *
 *  Compiler Compatibility::Transaction Descriptor Management:
 *
 *  TM_GET_THREAD()  : for getting the thread's descriptor, if needed
 *  TM_ARG_ALONE     : for passing descriptors to transactional functions
 *  TM_ARG           : (same)
 *  TM_PARAM         : (same)
 *  TM_PARAM_ALONE   : (same)
 *
 *  Compiler Compatibility::Annotations (unused in library):
 *
 *  TM_WAIVER        : mark a block that does not get TM instrumentation
 *  TM_CALLABLE      : mark a function as being callable by TM
 */

#ifndef API_LIBRARY_HPP__
#define API_LIBRARY_HPP__

#include <setjmp.h>
#include <stm/config.h>
#include <common/platform.hpp>
#include <stm/txthread.hpp>
//YZ
#include <map>
#include <stdlib.h>
#include <string.h>
#include <stm/mod.h>
#include <pthread.h>
//YZ
namespace stm
{
	//template <typename T>
	//inline T stm_read(T* addr, TxThread* thread, bool *last_abort)
	//{
	//    return DISPATCH<T, sizeof(T)>::read(addr, thread, last_abort);
	//}
	//
	//template <typename T>
	//inline void stm_write(T* addr, T val, TxThread* thread, bool *last_abort)
	//{
	//    DISPATCH<T, sizeof(T)>::write(addr, val, thread, last_abort);
	//}

	//TM_INLINE
	//inline void commit(TxThread* tx, bool *last_abort)
	//{
	//    // don't commit anything if we're nested... just exit this scope
	//    if (--tx->nesting_depth)
	//        return;
	//
	//    // dispatch to the appropriate end function
	//	#ifdef STM_PROTECT_STACK
	//    void* top_of_stack;
	//    tx->tmcommit(tx, &top_of_stack, last_abort);
	//	#else
	//    tx->tmcommit(tx, last_abort);
	//	#endif
	//
	//    // zero scope (to indicate "not in tx")
	//    CFENCE;
	//    tx->scope = NULL;
	//
	//    // record start of nontransactional time
	//    tx->end_txn_time = tick();
	//}

#define MY_TM_READ(var)       stm::stm_read(&var, tx, &last_abort)
#define MY_TM_WRITE(var, val) stm::stm_write(&var, val, tx, &last_abort)
#define MY_TM_PARAM , tx, &last_abort

#define TM_BEGIN(TYPE)                          \
{                                                       \
	stm::TxThread* tx = (stm::TxThread*)stm::Self;          \
	static tm_hist_map tm_hist[100]; \
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
		tm_hist[tx->id][tm].abort++;	\
		pthread_mutex_lock(&lock3);			\
		tm_abort[cur_tm][abort_flags]++;	\
		pthread_mutex_unlock(&lock3);			\
		pthread_mutex_lock(&lock2);			\
		active_tm[cur_tm]--;	\
		pthread_mutex_unlock(&lock2);			\
	}	\
	tm_hist[tx->id][tm].tot++;								\
	pthread_mutex_lock(&lock1);			\
	while(1) \
	{	\
		bool can_enter = true;	\
		for(int i = 0;i < MAX_TM_NUM;i++)	\
		{	\
			pthread_mutex_lock(&lock3);			\
			unsigned long temp_abort = tm_abort[cur_tm][i];	\
			pthread_mutex_unlock(&lock3);			\
			pthread_mutex_lock(&lock2);			\
			unsigned long temp_active = active_tm[i];	\
			pthread_mutex_unlock(&lock2);			\
			if(temp_abort && temp_active )	\
			{	\
				can_enter = false;	\
				break;	\
			}	\
		}	\
		if(can_enter) break;	\
	}	\
	pthread_mutex_lock(&lock2);			\
	active_tm[cur_tm]++;	\
	pthread_mutex_unlock(&lock2);			\
	pthread_mutex_unlock(&lock1);			\
	stm::begin(tx, &_jmpbuf, abort_flags);        	\
	CFENCE;                                                 \
{

#define TM_END                               \
}                                           \
	stm::commit(tx);                            \
	pthread_mutex_lock(&lock2);			\
	active_tm[cur_tm]--;	\
	pthread_mutex_unlock(&lock2);			\
}
}

namespace stm
{
  /**
   *  Code to start a transaction.  We assume the caller already performed a
   *  setjmp, and is passing a valid setjmp buffer to this function.
   *
   *  The code to begin a transaction *could* all live on the far side of a
   *  function pointer.  By putting some of the code into this inlined
   *  function, we can:
   *
   *    (a) avoid overhead under subsumption nesting and
   *    (b) avoid code duplication or MACRO nastiness
   */
  TM_INLINE
  inline void begin(TxThread* tx, scope_t* s, uint32_t /*abort_flags*/)
  {
      if (++tx->nesting_depth > 1)
          return;

      // we must ensure that the write of the transaction's scope occurs
      // *before* the read of the begin function pointer.  On modern x86, a
      // CAS is faster than using WBR or xchg to achieve the ordering.  On
      // SPARC, WBR is best.
#ifdef STM_CPU_SPARC
      tx->scope = s; WBR;
#else
      // NB: this CAS fails on a transaction restart... is that too expensive?
      casptr((volatile uintptr_t*)&tx->scope, (uintptr_t)0, (uintptr_t)s);
#endif

      // some adaptivity mechanisms need to know nontransactional and
      // transactional time.  This code suffices, because it gets the time
      // between transactions.  If we need the time for a single transaction,
      // we can run ProfileTM
      if (tx->end_txn_time)
          tx->total_nontxn_time += (tick() - tx->end_txn_time);

      // now call the per-algorithm begin function
      TxThread::tmbegin(tx);
  }

  /**
   *  Code to commit a transaction.  As in begin(), we are using forced
   *  inlining to save a little bit of overhead for subsumption nesting, and to
   *  prevent code duplication.
   */
  TM_INLINE
  inline void commit(TxThread* tx)
  {
      // don't commit anything if we're nested... just exit this scope
      if (--tx->nesting_depth)
          return;

      // dispatch to the appropriate end function
#ifdef STM_PROTECT_STACK
      void* top_of_stack;
      tx->tmcommit(tx, &top_of_stack);
#else
      tx->tmcommit(tx);
#endif

      // zero scope (to indicate "not in tx")
      CFENCE;
      tx->scope = NULL;

      // record start of nontransactional time
      tx->end_txn_time = tick();
  }

  /**
   *  The STM system provides a message that exits the program (preferable to
   *  'assert(false)').  We use this in the API too, so it needs to be visible
   *  here
   */
  void NORETURN UNRECOVERABLE(const char*);

  /**
   *  This portion of the API addresses allocation.  We provide tx-safe malloc
   *  and free calls, which also work from nontransactional contexts.
   */

  /**
   *  get a chunk of memory that will be automatically reclaimed if the caller
   *  is a transaction that ultimately aborts
   */
  inline void* tx_alloc(size_t size) { return Self->allocator.txAlloc(size); }

  /**
   *  Free some memory.  If the caller is a transaction that ultimately aborts,
   *  the free will not happen.  If the caller is a transaction that commits,
   *  the free will happen at commit time.
   */
  inline void tx_free(void* p) { Self->allocator.txFree(p); }

  /**
   *  Master class for all objects that are used in transactions, to ensure
   *  that those objects have tx-safe allocation
   *
   *  WARNING:  DEPRECATED
   *
   *      We no longer use the Object class.  In G++ it is unsafe to call
   *      destructors from within a transaction (they trash the vtable
   *      pointer in an unrecoverable way!), and some compilers don't handle
   *      new() within a transaction correctly.  Ugly though it is, for now,
   *      you should just use malloc and free to create objects.
   */
  struct Object
  {
      void* operator new(size_t size) { return tx_alloc(size); }

      // it is never safe to call a destructor inside a tx with g++, because
      // the vtable will be overwritten; if the tx aborts, the vtable will not
      // be restored.  We hope this never gets called...
      void operator delete(void* ptr)
      {
          tx_free(ptr);
          UNRECOVERABLE("Calling destructors is not supported.");
      }
      virtual ~Object() { }
  };

  /**
   *  Here we declare the rest of the api to the STM library
   */

  /**
   *  Initialize the library (call before doing any per-thread initialization)
   *
   *  We rely on the default setjmp/longjmp abort handling when using the
   *  library API.
   */
  void sys_init(void (*abort_handler)(TxThread*) = NULL);

  /**
   *  Shut down the library.  This just dumps some statistics.
   */
  void sys_shutdown();

  /***  Set up a thread's transactional context */
  inline void thread_init() { TxThread::thread_init(); }

  /***  Shut down a thread's transactional context */
  inline void thread_shutdown() { TxThread::thread_shutdown(); }

  /**
   *  Set the current STM algorithm/policy.  This should be called at the
   *  beginning of each program phase
   */
  void set_policy(const char*);

  /***  Report the algorithm name that was used to initialize libstm */
  const char* get_algname();

  /**
   *  Try to become irrevocable.  Call this from within a transaction.
   */
  bool become_irrevoc(STM_WHEN_PROTECT_STACK(void** top_of_stack));

#ifdef STM_PROTECT_STACK
#   define TM_BECOME_IRREVOC() ({    \
  void* top_of_stack; \
  stm::become_irrevoc(&top_of_stack); })
#else
#   define TM_BECOME_IRREVOC() stm::become_irrevoc()
#endif

  /**
   *  Abort the current transaction and restart immediately.
   */
  void restart();
}

/*** pull in the per-memory-access instrumentation framework */
#include "library_inst.hpp"

/**
 *  Now we can make simple macros for reading and writing shared memory, by
 *  using templates to dispatch to the right code:
 */
namespace stm
{
  template <typename T>
  inline T stm_read(T* addr, TxThread* thread)
  {
      return DISPATCH<T, sizeof(T)>::read(addr, thread);
  }

  template <typename T>
  inline void stm_write(T* addr, T val, TxThread* thread)
  {
      DISPATCH<T, sizeof(T)>::write(addr, val, thread);
  }
} // namespace stm

/**
 * Code should only use these calls, not the template stuff declared above
 */
#define TM_READ(var)       stm::stm_read(&var, tx)
#define TM_WRITE(var, val) stm::stm_write(&var, val, tx)

/**
 *  This is the way to start a transaction
 */
#define MY_TM_BEGIN(TYPE)                                      \
    {                                                       \
    stm::TxThread* tx = (stm::TxThread*)stm::Self;          \
    jmp_buf _jmpbuf;                                        \
    uint32_t abort_flags = setjmp(_jmpbuf);                 \
    stm::begin(tx, &_jmpbuf, abort_flags);                  \
    CFENCE;                                                 \
    {

/**
 *  This is the way to commit a transaction.  Note that these macros weakly
 *  enforce lexical scoping
 */
#define MY_TM_END                                  \
    }                                           \
    stm::commit(tx);                            \
    }

/**
 *  Macro to get STM context.  This currently produces a pointer to a TxThread
 */
#define TM_GET_THREAD() stm::TxThread* tx = (stm::TxThread*)stm::Self
#define TM_ARG_ALONE stm::TxThread* tx
#define TM_ARG , TM_ARG_ALONE
#define TM_PARAM , tx
#define TM_PARAM_ALONE tx

#define TM_WAIVER
#define TM_CALLABLE

#define TM_SYS_INIT()        stm::sys_init()
#define TM_THREAD_INIT       stm::thread_init
#define TM_THREAD_SHUTDOWN() stm::thread_shutdown()
#define TM_SYS_SHUTDOWN      stm::sys_shutdown
#define TM_ALLOC             stm::tx_alloc
#define TM_FREE              stm::tx_free
#define TM_SET_POLICY(P)     stm::set_policy(P)

#define TM_GET_ALGNAME()     stm::get_algname()

/**
 * This is gross.  ITM, like any good compiler, will make nontransactional
 * versions of code so that we can cleanly do initialization from outside of
 * a transaction.  The library **can** do this, but only via some cumbersome
 * template games that we really don't want to keep playing (see the previous
 * release for examples).
 *
 * Since we don't want to have transactional configuration (it is slow, and
 * it messes up some accounting of commits and transaction sizes), we use the
 * following trick: if we aren't using a compiler for instrumentation, then
 * BEGIN_FAST_INITIALIZATION will copy the current STM configuration (envar
 * STM_CONFIG) to a local, then switch the mode to CGL, then call the
 * instrumented functions using CGL instrumentation (e.g., the lightest
 * possible, and correct without a 'commit').  Likewise, if we aren't using a
 * compiler for instrumentation, then END_FAST_INITIALIZATION will restore
 * the original configuration, so that the app will use the STM as expected.
 */
#ifdef STM_API_ITM
#  define TM_BEGIN_FAST_INITIALIZATION()
#  define TM_END_FAST_INITIALIZATION()
#else
#  define TM_BEGIN_FAST_INITIALIZATION()                \
    const char* __config_string__ = TM_GET_ALGNAME();   \
    TM_SET_POLICY("CGL");                               \
    TM_GET_THREAD()
#  define TM_END_FAST_INITIALIZATION()          \
    TM_SET_POLICY(__config_string__)
#endif

#endif // API_LIBRARY_HPP__
