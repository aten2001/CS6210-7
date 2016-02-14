/**********************************************************************
gtthread_mutex.c.  

This file contains the implementation of the mutex subset of the
gtthreads library.  The locks can be implemented with a simple queue.
**********************************************************************/

/*
    Include as needed
*/


#include "gtthread.h"

/*
    The gtthread_mutex_init() function is analogous to
    pthread_mutex_init with the default parameters enforced.
    There is no need to create a static initializer analogous to
    PTHREAD_MUTEX_INITIALIZER.
 */
int gtthread_mutex_init(gtthread_mutex_t* mutex){
    sigprocmask(SIG_BLOCK,&vtalrm,NULL);
    steque_init(&mutex->block_queue);
    mutex->status = FREE;
    mutex->current_holder = NOBODY;
    sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
    return 0;
}
    
/*
  The gtthread_mutex_lock() is analogous to pthread_mutex_lock.
  Returns zero on success.
 */
int gtthread_mutex_lock(gtthread_mutex_t* mutex){
    sigprocmask(SIG_BLOCK,&vtalrm,NULL);
    if(mutex->status == FREE){
        mutex->status = BUSY;
        mutex->current_holder = current_thread;
    }
    else{
        steque_enqueue(&(mutex->block_queue),current_thread);
        current_thread->status = SLEEP;
        scheduler(0);
    }
    sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
    return 0;
}

/*
  The gtthread_mutex_unlock() is analogous to pthread_mutex_unlock.
  Returns zero on success.
 */
int gtthread_mutex_unlock(gtthread_mutex_t *mutex){
    sigprocmask(SIG_BLOCK,&vtalrm,NULL);
    if(mutex->status == FREE || mutex->current_holder != current_thread){
        sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
        return 1;
    }
    while(1){
        if(steque_isempty(&(mutex->block_queue))){
            mutex->current_holder = NOBODY;
            mutex->status = FREE;
        }
        else{
            gtthread_t * temp = steque_pop(&(mutex->block_queue));
            if(temp->status == DIE) continue;
            temp->status = ACTIVED;
            steque_enqueue(&gtthread_queue, temp);
            mutex->current_holder = temp;
            mutex->status = BUSY;
        }
        break;
    }
    sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
    return 0;
}

/*
  The gtthread_mutex_destroy() function is analogous to
  pthread_mutex_destroy and frees any resourcs associated with the mutex.
*/
int gtthread_mutex_destroy(gtthread_mutex_t *mutex){
    if(mutex->status == BUSY)
        return EBUSY;
    steque_destroy(&(mutex->block_queue));
    return 0;
}
