#include <eeros/core/ExecutorService.hpp>

#include <cstdlib>
#include <iostream>
#include <ostream>
#include <pthread.h>
#include <time.h>
#include <sched.h>
#include <sys/mman.h>
#include <string.h>

#define NSEC_PER_SEC (1000000000) /* The number of nsecs per sec. */

int ExecutorService::nofThreads = 0;
pthread_t ExecutorService::threads[MAX_NOF_THREADS] = {0, 0, 0, 0, 0, 0, 0, 0};

int ExecutorService::createNewThread(Executor* e) {
	int threadId = nofThreads;
	int ret;
	ret = pthread_create(&ExecutorService::threads[ExecutorService::nofThreads++], NULL, ExecutorService::threadAction, (void*)e);
	std::cout << "Thread (#" << threadId << ") created with return value " << ret << std::endl;
	return threadId;
}

/**** TODO check if simulation or not! ****/

void* ExecutorService::threadAction(void* ptr) {
	Executor* e = (Executor*) ptr;
	struct timespec t;

	int interval = (int)(e->period * NSEC_PER_SEC); /* s -> ns */

	clock_gettime(CLOCK_MONOTONIC ,&t);

	while(e->status != kStop) {
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
		e->run();
		t.tv_nsec += interval;
		while (t.tv_nsec >= NSEC_PER_SEC) {
			t.tv_nsec -= NSEC_PER_SEC;
			t.tv_sec++;
		}
	}
	std::cout << "Thread finished" << std::endl;
	e->status = kStopped;
}

void ExecutorService::waitForSequenceEnd(Executor* waitExecutor) {
	if(waitExecutor && waitExecutor->getStatus() == kRunning){
		pthread_join(threads[waitExecutor->getThreadId()], NULL);
	}
}