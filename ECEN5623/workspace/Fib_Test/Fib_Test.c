
/****************************************************************************/
/*                                                                          */
/* Sean Donohue                                                             */
/*                                                                          */
/****************************************************************************/

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <sched.h>
#include <time.h>
#include <stdlib.h>
#include <semaphore.h>

#define FIB_LIMIT_FOR_32_BIT	47
#define NSEC_PER_SEC		(1000000000)
#define DELAY_TICKS (1)
#define ERROR (-1)
#define OK (0)

pthread_t fib10_thread;
pthread_attr_t fib10_attr;
struct sched_param fib10_param;
sem_t fib10_sem;

pthread_t fib20_thread;
pthread_attr_t fib20_attr;
struct sched_param fib20_param;
sem_t fib20_sem;

pthread_t sched_thread;
pthread_attr_t sched_attr;
struct sched_param sched_param;

int abortTest = 0;
unsigned int seqIterations = FIB_LIMIT_FOR_32_BIT;
unsigned int idx = 0, jdx = 1, fib = 0, fib0 = 0, fib1 = 1,
	fib10Cnt = 0, fib20Cnt = 0;
char ciMarker[] = "CI";

#define FIB_TEST(seqCnt, iterCnt) 		\
	for(idx = 0; idx < iterCnt; idx++)	\
	{					\
		fib = fib0 + fib1;		\
		while (jdx < seqCnt)		\
		{				\
			fib0 = fib1;		\
			fib1 = fib;		\
			fib = fib0 + fib1;	\
			jdx++;			\
		}				\
	}					\

int delta_t(struct timespec *stop, struct timespec *start, struct timespec *delta_t)
{
  int dt_sec=stop->tv_sec - start->tv_sec;
  int dt_nsec=stop->tv_nsec - start->tv_nsec;

  if(dt_sec >= 0)
  {
    if(dt_nsec >= 0)
    {
      delta_t->tv_sec=dt_sec;
      delta_t->tv_nsec=dt_nsec;
    }
    else
    {
      delta_t->tv_sec=dt_sec-1;
      delta_t->tv_nsec=NSEC_PER_SEC+dt_nsec;
    }
  }
  else
  {
    if(dt_nsec >= 0)
    {
      delta_t->tv_sec=dt_sec;
      delta_t->tv_nsec=dt_nsec;
    }
    else
    {
      delta_t->tv_sec=dt_sec-1;
      delta_t->tv_nsec=NSEC_PER_SEC+dt_nsec;
    }
  }

  return(OK);
}

void *fib10(void *threadID)
{
	unsigned long mask = 1;	// Processor 0
	struct timespec rtclk_dt = {0, 0};
	struct timespec rtclk_start_time = {0, 0};
	struct timespec rtclk_stop_time = {0, 0};

	// Bind this thread to core 0.
	if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0)
		perror("pthread_setaffinity_np");

	while(!abortTest)
	{
		sem_wait(&fib10_sem);
		clock_gettime(CLOCK_REALTIME, &rtclk_start_time);
		FIB_TEST(seqIterations, 710000);
		clock_gettime(CLOCK_REALTIME, &rtclk_stop_time);
		delta_t(&rtclk_stop_time, &rtclk_start_time, &rtclk_dt);
		printf("Fib10 Runtime: %f ms\n", ((double) (rtclk_dt.tv_nsec)) / 1000000.0);
		fib10Cnt++;
	}
}

void *fib20(void *threadID)
{
	unsigned long mask = 1;	// Processor 0
	struct timespec rtclk_dt = {0, 0};
	struct timespec rtclk_start_time = {0, 0};
	struct timespec rtclk_stop_time = {0, 0};

	// Bind this thread to core 0.
	if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0)
		perror("pthread_setaffinity_np");

	while(!abortTest)
	{
		sem_wait(&fib20_sem);
		clock_gettime(CLOCK_REALTIME, &rtclk_start_time);
		FIB_TEST(seqIterations, 1550000);
		clock_gettime(CLOCK_REALTIME, &rtclk_stop_time);
		delta_t(&rtclk_stop_time, &rtclk_start_time, &rtclk_dt);
		printf("Fib20 Runtime: %f ms\n", ((double) (rtclk_dt.tv_nsec)) / 1000000.0);
		fib20Cnt++;
	}
}

void *scheduler(void *threadID)
{
	int rc;
	unsigned long mask = 1; // Processor 0
	useconds_t sleep_10us = 10000, sleep_20us = 20000;

	// Bind this thread to core 0.
	if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0)
		perror("pthread_setaffinity_np");

	// Create the threads
	rc = pthread_create(&fib10_thread, &fib10_attr, fib10, (void *) 0);
	if (rc)
	{
		printf("ERROR; Failed to create fib10 thread, rc is %d\n", rc);
		perror("pthread_create"); exit(-1);
	}

	rc = pthread_create(&fib20_thread, &fib20_attr, fib20, (void *) 0);
	if (rc)
	{
		printf("ERROR; Failed to create fib20 thread, rc is %d\n", rc);
		perror("pthread_create"); exit(-1);
	}

    // Wait for the threads to finish initialization
	usleep(sleep_10us);

	// Post the semaphore to each thread
	sem_post(&fib10_sem);
	sem_post(&fib20_sem);

	//while(!abortTest)
	{
		usleep(sleep_20us); sem_post(&fib10_sem);
		usleep(sleep_20us); sem_post(&fib10_sem);
		usleep(sleep_10us); sem_post(&fib20_sem);
		usleep(sleep_10us); sem_post(&fib10_sem);
		usleep(sleep_20us); sem_post(&fib10_sem);

		//usleep(sleep_20us);
		//sem_post(&fib10_sem); sem_post(&fib20_sem);
	}
}

void testFib10_Runtime(void)
{
	int rc;

	struct timespec rtclk_dt = {0, 0};
	struct timespec rtclk_start_time = {0, 0};
	struct timespec rtclk_stop_time = {0, 0};
	struct timespec delay_error = {0, 0};

	rc = pthread_create(&fib10_thread, &fib10_attr, fib10, (void *)0);
    if (rc)
    {
        printf("ERROR; pthread_create() rc is %d\n", rc);
        perror("pthread_create");
        exit(-1);
    }

	while (fib10Cnt < 10)
	{
        sem_post(&fib10_sem);
        usleep(10000);
	}
}

void testFib20_Runtime(void)
{
	int rc;

	struct timespec rtclk_dt = {0, 0};
	struct timespec rtclk_start_time = {0, 0};
	struct timespec rtclk_stop_time = {0, 0};
	struct timespec delay_error = {0, 0};

    rc = pthread_create(&fib20_thread, &fib20_attr, fib20, (void *)0);
    if (rc)
    {
        printf("ERROR; pthread_create() rc is %d\n", rc);
        perror("pthread_create");
        exit(-1);
    }

	while (fib20Cnt < 10)
	{
        sem_post(&fib20_sem);
        usleep(20000);
	}
	abortTest = 1;
}

int main(void)
{
	int rc;

	// Initialize semaphores
	sem_init(&fib10_sem, 0, 1);
	sem_init(&fib20_sem, 0, 1);

	// Assign fixed priority scheduling to the Fib10, Fib20 and the scheduler thread
	pthread_attr_init(&fib10_attr);
	pthread_attr_setinheritsched(&fib10_attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&fib10_attr, SCHED_FIFO);

	pthread_attr_init(&fib20_attr);
	pthread_attr_setinheritsched(&fib20_attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&fib20_attr, SCHED_FIFO);

	pthread_attr_init(&sched_attr);
	pthread_attr_setinheritsched(&sched_attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&sched_attr, SCHED_FIFO);

	// Assign highest priority to Scheduler, followed by Fib10 then Fib20
	// Max priority will be 99, so subsequent threads will have slightly lower priority
	sched_param.sched_priority = sched_get_priority_max(SCHED_FIFO);
	fib10_param.sched_priority = sched_param.sched_priority - 1;
	fib20_param.sched_priority = fib10_param.sched_priority - 1;

	// Apply the scheduling attributes
	rc = sched_setscheduler(getpid(), SCHED_FIFO, &sched_param);
	rc |= sched_setscheduler(getpid(), SCHED_FIFO, &fib10_param);
	rc |= sched_setscheduler(getpid(), SCHED_FIFO, &fib20_param);
	if (rc)
	{
		printf("ERROR; sched_setscheduler rc is %d\n", rc);
		perror("sched_setscheduler"); exit(-1);
	}
	pthread_attr_setschedparam(&sched_attr, &sched_param);
	pthread_attr_setschedparam(&fib10_attr, &fib10_param);
	pthread_attr_setschedparam(&fib20_attr, &fib20_param);

	//testFib10_Runtime();
	//testFib20_Runtime();
	rc = pthread_create(&sched_thread, &sched_attr, scheduler, (void *) 0);
	if (rc)
	{
		printf("ERROR; failed to create scheduler thread. rc is %d\n", rc);
		perror("pthread_create"); exit(-1);
	}

	pthread_join(sched_thread, NULL);

	// I don't know why I need to do this yet.
	if (pthread_attr_destroy(&fib10_attr) != 0)
		perror("attr destroy");
	if (pthread_attr_destroy(&fib20_attr) != 0)
		perror("attr destroy");
	if (pthread_attr_destroy(&sched_attr) != 0)
		perror("attr destroy");

	return 0;
}
