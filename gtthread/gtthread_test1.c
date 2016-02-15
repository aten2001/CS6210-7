#include <stdio.h>
#include <stdlib.h>
#include "gtthread.h"

/* Tests creation.
   Should print "Hello World!" */
gtthread_t maint;

void *thr1(void *in) {
  printf("Hello World! %d\n",*(int*)in);
  fflush(stdout);
  sleep(2);
  //gtthread_yield();
  printf("*******************  1\n");
  sleep(2);
  printf("*******************  2\n");
  sleep(2);
  printf("*******************  3\n");
  sleep(2);
  printf("*******************  4\n");
  sleep(2);
  printf("*******************  5\n");
  //gtthread_cancel(gtthread_self());
  
  //return in;
  // or these
  int x=123;
  gtthread_exit(&x);
}
void *thr2(void *in) {
  void *joinret;
  int test;
  //printf("Goodbye Cruel World!\n");
  printf("Goodbye World! %f\n",*(double*)in);
  printf("++++++++++++++  1\n");
  sleep(2);
  printf("++++++++++++++  2\n");
  sleep(2);
  printf("++++++++++++++  3\n");
  sleep(2);
  printf("++++++++++++++  4\n");
  sleep(2);
  printf("++++++++++++++  5\n");
  
  //gtthread_yield();
  test = gtthread_join(maint,&joinret);
  if (test!=0){
	  printf("join error %d\n",test);
  }
  else {
	  printf("main returned value %d\n",*(int*)joinret);
  }
  //while(1){
	//sleep(10);
  //}
  fflush(stdout);
  int x=234;
  gtthread_exit(&x);
  //gtthread_exit(in);
  return NULL;
}


int main() {
  gtthread_t t1,t2,t3;
  int c = 37;
  double d = 3.14;
  int ret=666;
  void *joinret;
  //void *joinret2;
  joinret = NULL;
  //joinret2=NULL;
  int test;
  
  gtthread_init(100);
  maint=gtthread_self();
  printf("main thread id = %d\n",maint.id);
  gtthread_create( &t1, thr1, &c);
  gtthread_create( &t2, thr2, &d);
  //gtthread_cancel(t3);
  //gtthread_yield();
  test=gtthread_join(t1,&joinret);
  if (test!=0){
	  printf("join error %d\n",test);
  }
  else {
	  printf("t1 return value %d\n",*(int*)joinret);
  }
  
  //gtthread_yield();
  //gtthread_yield();
  gtthread_exit(&ret);
  return EXIT_SUCCESS;
}