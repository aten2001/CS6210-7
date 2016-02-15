CC = gcc            # default is CC = cc
CFLAGS = -g -Wall   # default is CFLAGS = [blank]

GTTHREADS_SRC = gtthread_sched_new.c gtthread_mutex.c steque.c
GTTHREADS_OBJ = $(patsubst %.c,%.o,$(GTTHREADS_SRC))

# pattern rule for object files
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

#### Producer-Consumer ####
producer_consumer: producer_consumer.o steque.o
	$(CC) -o producer_consumer producer_consumer.o steque.o -lpthread

#### Dining ####
dining_main: dining_main.o
	$(CC) -o dining_main dining_main.o -lpthread

#### GTThreads ####
gtthread_main: gtthread_main.o $(GTTHREADS_OBJ)
	$(CC) -o gtthread_main gtthread_main.o $(GTTHREADS_OBJ)

gtthread_test1: gtthread_test1.o $(GTTHREADS_OBJ)
	$(CC) -o gtthread_test1 gtthread_test1.o $(GTTHREADS_OBJ)

gtthread_test2: gtthread_test2.o $(GTTHREADS_OBJ)
	$(CC) -o gtthread_test2 gtthread_test2.o $(GTTHREADS_OBJ)

gtthread_test3: gtthread_test3.o $(GTTHREADS_OBJ)
	$(CC) -o gtthread_test3 gtthread_test3.o $(GTTHREADS_OBJ)

gtthread_test4: gtthread_test4.o $(GTTHREADS_OBJ)
	$(CC) -o gtthread_test4 gtthread_test4.o $(GTTHREADS_OBJ)

gtthread_test5: gtthread_test5.o $(GTTHREADS_OBJ)
	$(CC) -o gtthread_test5 gtthread_test5.o $(GTTHREADS_OBJ)

gtthread_test6: gtthread_test6.o $(GTTHREADS_OBJ)
	$(CC) -o gtthread_test6 gtthread_test6.o $(GTTHREADS_OBJ)

gtthread_test7: gtthread_test7.o $(GTTHREADS_OBJ)
	$(CC) -o gtthread_test7 gtthread_test7.o $(GTTHREADS_OBJ)

gtthread_test8: gtthread_test8.o $(GTTHREADS_OBJ)
	$(CC) -o gtthread_test8 gtthread_test8.o $(GTTHREADS_OBJ)

gtthread_test9: gtthread_test9.o $(GTTHREADS_OBJ)
	$(CC) -o gtthread_test9 gtthread_test9.o $(GTTHREADS_OBJ)

test_join1: test_join1.o $(GTTHREADS_OBJ)
	$(CC) -o test_join1 test_join1.o $(GTTHREADS_OBJ)

producer_consumer_gtthread: producer_consumer_gtthread.o $(GTTHREADS_OBJ)
	$(CC) -o producer_consumer_gtthread producer_consumer_gtthread.o $(GTTHREADS_OBJ)

clean:
	$(RM) -f *.o producer_consumer dining_main gtthread_main gtthread_test1 test_join1
