//This method prevent deadlock by dividing philosophers into even and odd groups.
//They have oppsite way to pick up left and right chopstick.
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include "philosopher.h"
pthread_mutex_t mutex[5];
/*
 * Performs necessary initialization of mutexes.
 */
void chopsticks_init(){
	int i;
	for (i = 0; i < 5; i++){
		assert(!pthread_mutex_init(&mutex[i],NULL));
    }
}

/*
 * Cleans up mutex resources.
 */
void chopsticks_destroy(){
	int i;
	for (i = 0; i < 5; i++){
		assert(!pthread_mutex_destroy(&(mutex[i])));
	}
}

/*
 * Uses pickup_left_chopstick and pickup_right_chopstick
 * to pick up the chopsticks
 */   
void pickup_chopsticks(int phil_id){
	
	if (phil_id % 2 == 0) {
	
		pthread_mutex_lock(&mutex[(phil_id+1)%5]);
		pickup_right_chopstick(phil_id);
		
		pthread_mutex_lock(&mutex[(phil_id)%5]);
		pickup_left_chopstick(phil_id);
	} else {
	
		pthread_mutex_lock(&mutex[(phil_id)%5]);
		pickup_left_chopstick(phil_id);
		
		pthread_mutex_lock(&mutex[(phil_id+1)%5]);
		pickup_right_chopstick(phil_id);
	}
}

/*
 * Uses pickup_left_chopstick and pickup_right_chopstick
 * to pick up the chopsticks
 */   
void putdown_chopsticks(int phil_id){
	if (phil_id % 2 == 0) {
		
		putdown_right_chopstick(phil_id);
		pthread_mutex_unlock(&mutex[(phil_id+1)%5]);
		
		putdown_left_chopstick(phil_id);
		pthread_mutex_unlock(&mutex[(phil_id)%5]);
		
		
	} else {
	
		putdown_left_chopstick(phil_id);
		pthread_mutex_unlock(&mutex[(phil_id)%5]);		
		
		putdown_right_chopstick(phil_id);
		pthread_mutex_unlock(&mutex[(phil_id+1)%5]);;
	}	
}
