#ifndef GTTHREAD_H
#define GTTHREAD_H
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <ucontext.h>
#include "steque.h"
//#define DEBUG
#define GTTHREAD_CANCELLED 0
#define NOBODY 0
#define EBUSY 1
sigset_t vtalrm;
/* Define gtthread_t and gtthread_mutex_t types here */
enum gtthread_status
{
    	ACTIVED,
    	SLEEP, //blocked or join other thread
    	DIE,
};
enum mutex_status
{
        FREE,
        BUSY, 
};
typedef struct gtthread_t{
	int id;
	enum gtthread_status status;
	ucontext_t context;
    void* retval;
    steque_t join_queue;
	
}gtthread_t;

typedef struct{
    gtthread_t * current_holder;
    enum mutex_status status;
    steque_t block_queue;
}gtthread_mutex_t;

int thread_pool_size;
int id;
struct itimerval *T;
int global_period;
steque_t gtthread_queue;
gtthread_t ** thread_pool;
gtthread_t * current_thread;
gtthread_t * main_thread;
ucontext_t schedule_context;
ucontext_t going_to_die_context;
void scheduler(int sig);
void gtthread_init(long period);
int  gtthread_create(gtthread_t *thread,
                     void *(*start_routine)(void *),
                     void *arg);
int  gtthread_join(gtthread_t thread, void **status);
void gtthread_exit(void *retval);
void gtthread_yield(void);
int  gtthread_equal(gtthread_t t1, gtthread_t t2);
int  gtthread_cancel(gtthread_t thread);
gtthread_t gtthread_self(void);
int  gtthread_mutex_init(gtthread_mutex_t *mutex);
int  gtthread_mutex_lock(gtthread_mutex_t *mutex);
int  gtthread_mutex_unlock(gtthread_mutex_t *mutex);
int  gtthread_mutex_destroy(gtthread_mutex_t *mutex);
#endif
