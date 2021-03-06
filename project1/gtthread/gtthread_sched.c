/**********************************************************************
gtthread_sched.c.  

This file contains the implementation of the scheduling subset of the
gtthreads library.  A simple round-robin queue should be used.
 **********************************************************************/
/*
  Include as needed
*/
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "gtthread.h"


/* 
   Students should define global variables and helper functions as
   they see fit.
 */

void schedule(void){
	//sigprocmask(SIG_BLOCK,&vtalrm,NULL);
	printf("enter schedule function\n");
	fflush(stdout);
	
	while(!steque_isempty(&gtthread_queue)){
		/*PRINT FUNCTION*/
		steque_node_t* node = gtthread_queue.front;
		printf("list \n"); 
		while (node != NULL){
			printf("%d ",((gtthread_t *)(node->item))->id);
			node = node->next;	
		}
		printf("\n");
		fflush(stdout);
		/*PRINT FUNCTION END*/
		
    		gtthread_t * thread = steque_pop(&gtthread_queue);

    		if (thread->status == DIE){
			printf("die to here\n");
			if(steque_isempty(&gtthread_queue)) break;
    		}
    		else if(thread->status == SLEEP){
      			//some operations for join thread
    		}
    		else {//still runable
      			steque_enqueue(&gtthread_queue, thread);
    		}
		current_thread = (gtthread_t *)steque_front(&gtthread_queue);
		printf("swap out of schedule function\n");
		fflush(stdout);
		sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
    	swapcontext(&schedule_context,&(((gtthread_t *)steque_front(&gtthread_queue))->context));
    	sigprocmask(SIG_BLOCK,&vtalrm,NULL);
    while(1){
	int i = 0;
	for(; i < 1000000; i ++){
		if(i%100000 == 0) printf("%d\n",i );
	}
}
		printf("swap into schedule function\n");
		fflush(stdout);
		
  	}
	printf("leave schedule, whole program exit\n");
	//sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
	swapcontext(&schedule_context,&going_to_die_context);
}




/*
  The gtthread_init() function does not have a corresponding pthread equivalent.
  It must be called from the main thread before any other GTThreads
  functions are called. It allows the caller to specify the scheduling
  period (quantum in micro second), and may also perform any other
  necessary initialization.  If period is zero, then thread switching should
  occur only on calls to gtthread_yield().

  Recall that the initial thread of the program (i.e. the one running
  main() ) is a thread like any other. It should have a
  gtthread_t that clients can retrieve by calling gtthread_self()
  from the initial thread, and they should be able to specify it as an
  argument to other GTThreads functions. The only difference in the
  initial thread is how it behaves when it executes a return
  instruction. You can find details on this difference in the man page
  for pthread_create.
 */
void alrm_handler(int sig){
	//sigprocmask(SIG_BLOCK,&vtalrm,NULL);

	printf("enter alarm handler\n"); fflush(stdout);	
	swapcontext(&(current_thread->context),&schedule_context);
	printf("thread %d come back\n",current_thread->id); fflush(stdout);	
	// T->it_value.tv_usec = T->it_interval.tv_usec = global_period;
	// setitimer(ITIMER_VIRTUAL, T, NULL);
	//sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
}
int timer_init(long period){
	global_period = period;
	if (period == 0) 
		return 0;
  struct sigaction act;
  
/*
  Setting up the signal mask
  */
  sigemptyset(&vtalrm);
  sigaddset(&vtalrm, SIGVTALRM);
  sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);

  /* 
     Setting up the alarm
  */
  T = (struct itimerval*) malloc(sizeof(struct itimerval));
  T->it_value.tv_sec = T->it_interval.tv_sec = 0;
  T->it_value.tv_usec = T->it_interval.tv_usec = period;

  setitimer(ITIMER_VIRTUAL, T, NULL);

  
  /*
    Setting up the handler
  */
  memset (&act, '\0', sizeof(act));
  act.sa_handler = &alrm_handler;
  if (sigaction(SIGVTALRM, &act, NULL) < 0) {
    perror ("sigaction");
    return 1;
  }
		printf("set %ld successful\n",period);
  	return 0;
}


/*
  The gtthread_create() function mirrors the pthread_create() function,
  only default attributes are always assumed.
 */
void thread_create_helper(gtthread_t * thread){	
	thread->status = ACTIVED;
	sigprocmask(SIG_BLOCK,&vtalrm,NULL);
	thread->id = id;
	id = id + 1;
	sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
	getcontext(&(thread->context));
	(thread->context).uc_stack.ss_sp = (char*) malloc(SIGSTKSZ);;
	(thread->context).uc_stack.ss_size = SIGSTKSZ;

}
void start_routine_helper(gtthread_t * thread,void* (*func)(void *), void *arg){
	func(arg);
	printf("call exit\n");
	gtthread_exit(0);
}
void scheduler_create(){
	getcontext(&schedule_context);
	schedule_context.uc_stack.ss_sp = (char*) malloc(SIGSTKSZ);;
	schedule_context.uc_stack.ss_size = SIGSTKSZ;
	schedule_context.uc_link = &going_to_die_context;
	makecontext(&schedule_context,schedule,0);
}
void gtthread_main_thread_create(){
	gtthread_t * main_thread = (gtthread_t *)malloc(sizeof(gtthread_t));
	thread_create_helper(main_thread);
	(main_thread->context).uc_link = &schedule_context;
	current_thread = main_thread;
  	steque_enqueue(&gtthread_queue, main_thread);
}

int gtthread_create(gtthread_t *thread, void *(*start_routine)(void *), void *arg){
	
	thread_create_helper(thread);
	
	(thread->context).uc_link = &schedule_context;
	
	makecontext(&(thread->context),(void (*) (void))start_routine_helper,3, thread, start_routine, arg);
	
	sigprocmask(SIG_BLOCK,&vtalrm,NULL);
  	steque_enqueue(&gtthread_queue, thread);
	sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
	
}
void gtthread_init(long period){
	id = 0;
	steque_init(&gtthread_queue);
	printf("schedule init\n");
	scheduler_create();
	printf("main thread create\n");
	gtthread_main_thread_create();
	printf("set up timer and signal\n");
  	if(timer_init(period)){
    		perror ("sigaction error");
    		return;
  	}
}
/*
  The gtthread_join() function is analogous to pthread_join.
  All gtthreads are joinable.
 */
int gtthread_join(gtthread_t thread, void **status){


}

/*
  The gtthread_exit() function is analogous to pthread_exit.
 */

void gtthread_exit(void* retval){
	current_thread->status = DIE;
	printf("%d die\n", current_thread->id);
	if (current_thread->id == 0){ //main thread
		getcontext(&going_to_die_context);
		swapcontext(&going_to_die_context,&schedule_context);
		printf("whole program terminate!\n");
	}
}


/*
  The gtthread_yield() function is analogous to pthread_yield, causing
  the calling thread to relinquish the cpu and place itself at the
  back of the schedule queue.
 */
void gtthread_yield(void){
	//printf("begin yield\n");
	//fflush(stdout);
  	swapcontext(&(current_thread->context),&schedule_context);
	//printf("end yield\n");
	//fflush(stdout);
}

/*
  The gtthread_equal() function is analogous to pthread_equal,
  returning non-zero if the threads are the same and zero otherwise.
 */
int  gtthread_equal(gtthread_t t1, gtthread_t t2){
  if (t1.id == t2.id)
    return 1;
  else
    return 0;
}

/*
  The gtthread_cancel() function is analogous to pthread_cancel,
  allowing one thread to terminate another asynchronously.
 */
int  gtthread_cancel(gtthread_t thread){

}

/*
  Returns calling thread.
 */
gtthread_t gtthread_self(void){
	gtthread_t* thread;
	sigprocmask(SIG_BLOCK,&vtalrm,NULL);
	thread = current_thread;
	sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
  	return *thread;
}
