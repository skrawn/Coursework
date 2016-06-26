/*
 * main.c
 *
 *  Created on: Jun 25, 2016
 *      Author: skrawn
 */

#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>

#define NSEC_PER_SEC			1000000000
#define OK						0
#define ERROR					1

#define ATTITUDE_UPDATE_RATE	1	// Seconds
#define PRINT_ATTITUDE_RATE		3 	// Seconds
#define NUM_ITERATIONS			10

double attX = 0.0, attY = 0.0, attZ = 0.0;
struct timespec update_time;

pthread_mutex_t attitude_lock;
pthread_t printer, attitude_data;
pthread_attr_t printer_attr, attitude_attr;
struct sched_param printer_param, attitude_param;

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

void *UpdateAttitude(void *arg)
{
	unsigned long mask = 1;	// Processor 0

	// Bind this thread to core 0.
	if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0)
		perror("pthread_setaffinity_np");

	// Lock this data while it's updated
	pthread_mutex_lock(&attitude_lock);
	clock_gettime(CLOCK_REALTIME, &update_time);

	// Generate some random values. Be sure to not let this divide by 0!
	attX = ((double) rand()) / (((double) rand()) + 0.1);
	attY = ((double) rand()) / (((double) rand()) + 0.1);
	attZ = ((double) rand()) / (((double) rand()) + 0.1);

	printf("\nAttitude Updated! Time: %ds %dns\nX: %f Y: %f Z: %f\n",
			update_time.tv_sec, update_time.tv_nsec, attX, attY, attZ);

	// And then release it
	pthread_mutex_unlock(&attitude_lock);

	pthread_exit(NULL);
	return NULL;
}

void *PrintAttitude(void *arg)
{
	unsigned long mask = 1;	// Processor 0

	// Bind this thread to core 0.
	if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0)
		perror("pthread_setaffinity_np");

	// Lock the mutex
	pthread_mutex_lock(&attitude_lock);

	printf("\nAttitude Print - X: %f Y: %f Z: %f\n", attX, attY, attZ);
	printf("Last update: %ds %dns\n", update_time.tv_sec, update_time.tv_nsec);

	// And release it
	pthread_mutex_unlock(&attitude_lock);

	pthread_exit(NULL);
	return NULL;
}

int main(void)
{
	unsigned int iterations = 0, print_timer = 0, attitude_timer = 0;
	struct timespec sched_timer, last_time, delta_time;

	pthread_attr_t main_attr;
	pthread_t main_thread;
	struct sched_param main_param;

	// Initialize the mutex
	pthread_mutexattr_t att_attr;
	pthread_mutex_init(&attitude_lock, NULL);

	// Set up the scheduler for SCHED_FIFO
	pthread_attr_init(&printer_attr);
	pthread_attr_setinheritsched(&printer_attr, PTHREAD_INHERIT_SCHED);

	pthread_attr_init(&attitude_attr);
	pthread_attr_setinheritsched(&attitude_attr, PTHREAD_INHERIT_SCHED);

	// Make the main task SCHED FIFO
	main_thread = pthread_self();
	pthread_attr_init(&main_attr);
	pthread_attr_setinheritsched(&main_attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&main_attr, SCHED_FIFO);

	main_param.__sched_priority = sched_get_priority_max(SCHED_FIFO);
	attitude_param.__sched_priority = main_param.__sched_priority - 1;
	printer_param.__sched_priority = attitude_param.__sched_priority - 1;

	// Apply the scheduling attributes
	sched_setscheduler(getpid(), SCHED_FIFO, &main_param);
	sched_setscheduler(getpid(), SCHED_FIFO, &attitude_param);
	sched_setscheduler(getpid(), SCHED_FIFO, &printer_param);
	pthread_attr_setschedparam(&main_attr, &main_param);
	pthread_attr_setschedparam(&attitude_attr, &attitude_param);
	pthread_attr_setschedparam(&printer_attr, &printer_param);

	clock_gettime(CLOCK_REALTIME, &sched_timer);
	clock_gettime(CLOCK_REALTIME, &last_time);

	while (iterations < NUM_ITERATIONS)
	{
		clock_gettime(CLOCK_REALTIME, &sched_timer);
		delta_t(&sched_timer, &last_time, &delta_time);

		if (delta_time.tv_sec)
		{
			clock_gettime(CLOCK_REALTIME, &last_time);
			print_timer++; attitude_timer++;
		}

		if (print_timer >= PRINT_ATTITUDE_RATE)
		{
			pthread_create(&printer, &printer_attr, PrintAttitude, (void *) 0);

			print_timer = 0;

			// Whoever has the slower rate can update the iterations counter
			if (PRINT_ATTITUDE_RATE <= ATTITUDE_UPDATE_RATE)
				iterations++;
		}

		if (attitude_timer >= ATTITUDE_UPDATE_RATE)
		{
			pthread_create(&attitude_data, &attitude_attr, UpdateAttitude, (void *) 0);

			attitude_timer = 0;

			// Whoever has the slower rate can update the iterations counter
			if (PRINT_ATTITUDE_RATE >= ATTITUDE_UPDATE_RATE)
				iterations++;
		}
	}

	pthread_join(printer, NULL);
	pthread_join(attitude_data, NULL);

	pthread_mutex_destroy(&attitude_lock);

	return 0;
}
