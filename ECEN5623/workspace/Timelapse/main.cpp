/******************************************************************************
 * @file main.cpp
 * @brief Timelapse main file
 * @author Sean Donohue
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2016 Sean Donohue</b>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 *******************************************************************************/

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "bin_sem.hpp"
#include "capture.hpp"
#include "config.h"
#include "utils.hpp"

using namespace std;

#define N_RESOLUTIONS	5
#define N_FRAMES        100
#define NSEC_PER_SEC	1000000000

#define NSEC_TO_MSEC_F  1000000.0
#define NSEC_TO_MSEC    1000000
#define SEC_TO_MSEC		1000

#define N_TRANSFORMS    4
#define CANNY           0
#define SOBEL           1
#define HOUGH           2
#define CAPTURE_ONLY	3

#define DEADLINE_MS     100 // ms

pthread_t capture_thread;
pthread_attr_t capture_attr;
struct sched_param capture_param;

sem_t frame_signal;
//sem_t capture_complete;
bool test_complete = false;

int dev = 0;

int main( int argc, char** argv )
{
    struct timespec start_time, end_time, delta;
    unsigned int delta_ms;
    int retval, capture_id, deadline_misses = 0;
    cpu_set_t *cores;

	// Allocate one core for this thread
	cores = CPU_ALLOC(1);

	// Bind this thread to core 0.
	if (pthread_setaffinity_np(pthread_self(), sizeof(cores), cores) < 0)
		perror("pthread_setaffinity_np");

    pthread_t main_thread;
    pthread_attr_t main_attr;
    struct sched_param main_param;

    if(argc > 1)
    {
        sscanf(argv[1], "%d", &dev);
        printf("using %s\n", argv[1]);
    }
    else if(argc == 1)
        printf("using default\n");

    else
    {
        printf("usage: capture [dev]\n");
        exit(-1);
    }

    print_scheduler();

    // Set up the scheduler for SCHED_FIFO
    main_thread = pthread_self();
    pthread_attr_init(&main_attr);
    pthread_attr_setinheritsched(&main_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&main_attr, SCHED_FIFO);

    // Make child threads inherit the scheduler of the launching thread
    pthread_attr_init(&capture_attr);
    pthread_attr_setinheritsched(&capture_attr, PTHREAD_INHERIT_SCHED);

    // Set priorities
    //main_param.__sched_priority = sched_get_priority_max(SCHED_FIFO);
    //capture_param.__sched_priority = main_param.__sched_priority - 1;
    capture_param.__sched_priority = sched_get_priority_max(SCHED_FIFO);
    main_param.__sched_priority = capture_param.__sched_priority - 1;

    // Apply scheduling attributes
    sched_setscheduler(getpid(), SCHED_FIFO, &main_param);
    sched_setscheduler(getpid(), SCHED_FIFO, &capture_param);
    pthread_attr_setschedparam(&main_attr, &main_param);
    pthread_attr_setschedparam(&capture_attr, &capture_param);

    print_scheduler();

    if (!capture_init(dev))
    {
    	printf("Failed to open camera on device %d. Timelapse is closing\n", dev);
    	return -1;
    }

    printf("Capture initialized\n");

    //sem_init(&frame_signal, 1, 0);
    //sem_init(&capture_complete, 1, 0);

    //pthread_create(&capture_thread, &frame_attr, framerate_tests, (void *) 0);

    capture_id = pthread_create(&capture_thread, &capture_attr, capture_frame, (void *) 0);
    clock_gettime(CLOCK_REALTIME, &start_time);
    printf("main_loop\n");
    while (capture_get_capture_count() < 1000 && deadline_misses < 10)
    {
        clock_gettime(CLOCK_REALTIME, &end_time);
        delta_t(&end_time, &start_time, &delta);
        delta_ms = delta.tv_nsec / NSEC_TO_MSEC;

        if (delta_ms >= DEADLINE_MS)
        {

        	pthread_mutex_lock(&capture_complete_mutex);
        	pthread_mutex_unlock(&capture_complete_mutex);
        	//bin_sem_wait(&sem_capture_complete);
        	printf("Capture count = %lu\n", capture_get_capture_count());
        	capture_id = pthread_create(&capture_thread, &capture_attr, capture_frame, (void *) 0);
        	/*if (capture_get_capture_complete())
        	{
        		capture_id = pthread_create(&capture_thread, &capture_attr, capture_frame, (void *) 0);
        	}
        	else
        	{
        		deadline_misses++;
        		printf("Capture deadline missed!\n");
        	}*/

            clock_gettime(CLOCK_REALTIME, &start_time);
        }
    }

	// Free the core this thread is using
	CPU_FREE(cores);

	capture_close(dev);

	pthread_attr_destroy(&main_attr);
	pthread_attr_destroy(&capture_attr);

    //sem_destroy(&frame_signal);
    //sem_destroy(&capture_complete);

	return 0;
}
