#ifndef GTTHREAD_H
#define GTTHREAD_H
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <ucontext.h>
#include "steque.h"
sigset_t vtalrm;
/* Define gtthread_t and gtthread_mutex_t types here */
enum gtthread_status
{
    	ACTIVED,
    	SLEEP, //blocked or join other thread
    	DIE,
};
typedef struct gtthread_t{
	int id;
	enum gtthread_status status;
	ucontext_t context;
    	//char * stack;
	//char stack[4096];
	
}gtthread_t;

typedef struct{	
}gtthread_mutex_t;

int id;
struct itimerval *T;
int global_period;
steque_t gtthread_queue;
gtthread_t * current_thread;
gtthread_t * main_thread;
ucontext_t schedule_context;
ucontext_t going_to_die_context;

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
