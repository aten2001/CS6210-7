#include "gtthread.h"


gtthread_mutex_t m;

void child(void *a)
{
	int arg = (int) a;
	gtthread_mutex_lock(&m);
	int i = 0;
	for (; i < arg; i++) {
	    printf("child with arg %d loop with %d\n",arg,i);
	}
	gtthread_mutex_unlock(&m);
	gtthread_mutex_lock(&m);
	printf("Hello World");
	gtthread_mutex_unlock(&m);
	gtthread_yield();
	gtthread_mutex_lock(&m);
	gtthread_mutex_unlock(&m);

}

void parent(void *a)
{
    
}

int main()
{
   	gtthread_init(10);
   	gtthread_t t1, t2, t3, t4, t5;
   	gtthread_create(&t1,child,(void*) 10);
   	gtthread_create(&t2,child,(void*) 10);
   	gtthread_create(&t3,child,(void*) 10);
   	gtthread_create(&t4,child,(void*) 10);
   	gtthread_create(&t5,child,(void*) 10);
	gtthread_exit(0);
}