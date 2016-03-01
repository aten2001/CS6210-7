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
#include <assert.h>
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

void whole_program_terminate(){
	free((main_thread->context).uc_stack.ss_sp);
    free(main_thread);
    free(thread_pool);
    free(T);
    steque_destroy(&gtthread_queue);
#ifdef DEBUG
    printf("whole program terminate\n");
#endif
}
void release_join_queue(gtthread_t* thread){
	while(!steque_isempty(&(thread->join_queue))){
		gtthread_t * temp = steque_pop(&(thread->join_queue));
		if(temp->status == DIE) continue;
		assert(temp->status == SLEEP);
		temp->status = ACTIVED;
		steque_enqueue(&gtthread_queue,temp);
	}
	steque_destroy(&(thread->join_queue));
}
void scheduler(int sig){
#ifdef DEBUG
	printf("enter schedule function\n");
	print_queue();
#endif
    if (current_thread->status == DIE){
		if(current_thread != main_thread){
			free((current_thread->context).uc_stack.ss_sp);
		}
		release_join_queue(current_thread);
   	}
   	else if(current_thread->status == SLEEP){
     	//for join thread and block
     	//neccessary works has been done in gtthread_join and gtthread_mutex_lock, so nothing needs to do here
    }
    else {//still runable
      	steque_enqueue(&gtthread_queue, current_thread);
    }	

    gtthread_t* prev_thread = current_thread;
    while(1){// if the thread popped from queue is DIE, then it is cancelled by other thread and we needs to fetch a new one
    	if(steque_isempty(&gtthread_queue)){
    		if(current_thread == main_thread){//main thread exit at last
#ifdef DEBUG	
				printf("main thread exit last\n");
#endif    			
    			whole_program_terminate();
    			return;
    		}
    		else{
    			current_thread = main_thread;
    			setcontext(&(main_thread->context));
    		}
    	}

		gtthread_t* temp_thread = (gtthread_t *)steque_pop(&gtthread_queue);
		if(temp_thread->status != DIE) { // thread might be DIE since it could be cancelled by others.
			assert(temp_thread->status == ACTIVED);
			current_thread = temp_thread;
			break;
		}
	}

    swapcontext(&(prev_thread->context),&(current_thread->context));
	if(current_thread == main_thread && main_thread->status == DIE){
#ifdef DEBUG	
		printf("other thread exit last\n");
#endif 
		whole_program_terminate();
	}
#ifdef DEBUG	
	printf("leave schedule\n");
#endif

}

int timer_init(long period){
	global_period = period;
	if (period == 0) 
		return 0; //if period is 0, then no timer is set and thread switching should occur only on calls to gtthread_yield().
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
  	act.sa_handler = &scheduler;
  	if (sigaction(SIGVTALRM, &act, NULL) < 0) {
  		perror ("sigaction");
    	return 1;
  	}
#ifdef DEBUG
	printf("set %ld successful\n",period);
#endif
  	return 0;
}

void start_routine_helper(void* (*func)(void *), void *arg){
	sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
	void * retval = func(arg);
	gtthread_exit(retval);
}

void thread_create_helper(gtthread_t * thread){	
	thread->status = ACTIVED;	
	if(id == thread_pool_size){ //dynamically increase the thread pool size
		gtthread_t ** temp = (gtthread_t **)malloc(2*thread_pool_size*sizeof(gtthread_t*));
		memset(temp, 0, 2*thread_pool_size*sizeof(gtthread_t*));
		memcpy(temp, thread_pool, thread_pool_size*sizeof(gtthread_t*));
		free(thread_pool);
		thread_pool = temp;
		thread_pool_size *= 2;
	}
	thread_pool[id] = thread;
	thread->id = id;
	id = id + 1;	
	getcontext(&(thread->context));
	(thread->context).uc_stack.ss_sp = (char*) malloc(SIGSTKSZ);;
	(thread->context).uc_stack.ss_size = SIGSTKSZ;
	(thread->context).uc_link = NULL;
	steque_init(&(thread->join_queue));

}

void gtthread_main_thread_create(){
	sigprocmask(SIG_BLOCK,&vtalrm,NULL);
	main_thread = (gtthread_t *)malloc(sizeof(gtthread_t));
	thread_create_helper(main_thread);
	current_thread = main_thread;
	sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
}

int gtthread_create(gtthread_t *thread, void *(*start_routine)(void *), void *arg){
	sigprocmask(SIG_BLOCK,&vtalrm,NULL);
	thread_create_helper(thread);	
	makecontext(&(thread->context),(void (*) (void))start_routine_helper,2,start_routine, arg);		
  	steque_enqueue(&gtthread_queue, thread);
	sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
	return 0;
	
}
void gtthread_init(long period){
	id = 0;
	steque_init(&gtthread_queue);
	thread_pool = (gtthread_t **)malloc(4*sizeof(gtthread_t*));
	thread_pool_size = 4;
#ifdef DEBUG
	printf("main thread create\n");
#endif
	gtthread_main_thread_create();
#ifdef DEBUG
	printf("set up timer and signal\n");
#endif
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
	sigprocmask(SIG_BLOCK,&vtalrm,NULL);;
	if(thread.id >= id){
		sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
#ifdef DEBUG
		printf("[join] thread id  %d doesn't exist\n",thread.id);
#endif
		return 1; // thread doesn't exist
	}
	gtthread_t * joined_thread = thread_pool[thread.id];
	if(joined_thread->status != DIE){
		current_thread->status = SLEEP;
		steque_enqueue(&(joined_thread->join_queue),current_thread);
		scheduler(0);
	}
	if(status != NULL) 
		*status = joined_thread->retval;
	sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
	return 0;
}

/*
  The gtthread_exit() function is analogous to pthread_exit.
 */

void gtthread_exit(void* retval){
	sigprocmask(SIG_BLOCK,&vtalrm,NULL);
	current_thread->status = DIE;
	current_thread->retval = retval;
#ifdef DEBUG
	printf("%d die\n", current_thread->id);
#endif
	scheduler(0);
	sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
}


/*
  The gtthread_yield() function is analogous to pthread_yield, causing
  the calling thread to relinquish the cpu and place itself at the
  back of the schedule queue.
 */
void gtthread_yield(void){
	sigprocmask(SIG_BLOCK,&vtalrm,NULL);
#ifdef DEBUG
	printf("begin yield\n");
#endif

  	scheduler(0);
	
#ifdef DEBUG
	printf("end yield\n");
#endif
	sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
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
	sigprocmask(SIG_BLOCK,&vtalrm,NULL);;
	if(thread.id >= id){
		sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
#ifdef DEBUG
		printf("thread id  %d doesn't exist\n",thread.id);
#endif
		return 1; // thread doesn't exist
	}
	gtthread_t * cancelled_thread = thread_pool[thread.id];
	if(cancelled_thread == current_thread){
		sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
		gtthread_exit(GTTHREAD_CANCELLED);
	}
	else{
		cancelled_thread->status = DIE;
		cancelled_thread->retval = GTTHREAD_CANCELLED;
		release_join_queue(cancelled_thread);
#ifdef DEBUG
		printf("%d cancelled\n", cancelled_thread->id);
#endif
		sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
	}
	return 0;
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
