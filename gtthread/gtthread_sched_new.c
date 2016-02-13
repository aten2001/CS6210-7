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
void print_queue(){
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
}
void scheduler(void){
	//sigprocmask(SIG_BLOCK,&vtalrm,NULL);
	printf("enter schedule function\n");
	fflush(stdout);
	
	print_queue();

    if (current_thread->status == DIE){
		printf("die to here\n");
		//deal with threads join this current_thread
		//if(steque_isempty(&gtthread_queue)) break;
   	}
   	else if(current_thread->status == SLEEP){
     	//some operations for join thread and block thread
    }
    else {//still runable
      	steque_enqueue(&gtthread_queue, current_thread);
    }	

    if(steque_isempty(&gtthread_queue)){
    	if(current_thread == main_thread){//main thread exit at last
    		//delete main thread object
    		return;
    	}
    	else{
    		current_thread = main_thread;
    		setcontext(&(main_thread->context));
    	}
    }

    gtthread_t* prev_thread = current_thread;
	current_thread = (gtthread_t *)steque_pop(&gtthread_queue);
    swapcontext(&(prev_thread->context),&(current_thread->context));
	if(current_thread == main_thread && main_thread->status == DIE){
		free(main_thread); //since main thread object is newed, we need to delete main thread object here
		printf("whole program terminate\n");
	}
	else
		printf("leave schedule\n");
}

void alrm_handler(int sig){
	printf("enter alarm handler\n"); fflush(stdout);	
	scheduler();
	printf("thread %d come back\n",current_thread->id); fflush(stdout);	
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

void gtthread_main_thread_create(){
	main_thread = (gtthread_t *)malloc(sizeof(gtthread_t));
	thread_create_helper(main_thread);
	(main_thread->context).uc_link = NULL;
	current_thread = main_thread;
  	steque_enqueue(&gtthread_queue, main_thread);
}

int gtthread_create(gtthread_t *thread, void *(*start_routine)(void *), void *arg){
	
	thread_create_helper(thread);
	
	(thread->context).uc_link = NULL;
	
	makecontext(&(thread->context),(void (*) (void))start_routine_helper,3, thread, start_routine, arg);
	
	sigprocmask(SIG_BLOCK,&vtalrm,NULL);
  	steque_enqueue(&gtthread_queue, thread);
	sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
	
}
void gtthread_init(long period){
	id = 0;
	steque_init(&gtthread_queue);
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
	scheduler();
}


/*
  The gtthread_yield() function is analogous to pthread_yield, causing
  the calling thread to relinquish the cpu and place itself at the
  back of the schedule queue.
 */
void gtthread_yield(void){
	//printf("begin yield\n");
	//fflush(stdout);
  	scheduler();
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
