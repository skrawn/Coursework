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
#include <string>

#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "config.h"

#include "bin_sem.hpp"
#include "capture.hpp"
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

#define MAX_PATH_LENGTH		255
#define MAX_USERNAME_LEN	32

char working_directory[MAX_PATH_LENGTH] = {0};
const char capture_save_dir[] = "Captures\0";

pthread_t capture_thread;
pthread_attr_t capture_attr;
struct sched_param capture_param;

pthread_t vid_thread;
pthread_attr_t vid_attr;
struct sched_param vid_param;

sem_t frame_signal;
//sem_t capture_complete;
bool test_complete = false;

int dev = 0;
int fps;

int main( int argc, char** argv )
{
    struct timespec start_time, end_time, delta;
    struct stat st;
    unsigned int delta_ms;
    int retval, capture_id, deadline_misses = 0, i = MAX_PATH_LENGTH;
    cpu_set_t all_cpu_set, threadcpu;
    char process_path[MAX_PATH_LENGTH] = {0};

    // Determine the working directory of this process
    sprintf(process_path, "/proc/%d/exe", getpid());
    if (process_path != NULL)
    	readlink(process_path, working_directory, MAX_PATH_LENGTH);
    else
    {
    	strcpy(working_directory, "~/Documents");
    }

    // Returns the path to this process. Strip off the process name to get the path
    // then append the image saving directory.
    while (working_directory[i] != '/' && i > 0)
    {
    	i--;
    }
    i--;
    while (working_directory[i] != '/' && i > 0)
	{
		i--;
	}
    i--;
    while (working_directory[i] != '/' && i > 0)
	{
		i--;
	}
    memset(&working_directory[i+1], 0, MAX_PATH_LENGTH - (i+1));
    strcpy(&working_directory[i+1], capture_save_dir);
    printf("Working directory: %s\n", working_directory);

    // Create the directory if it does not exist
    if (stat(working_directory, &st))
    {
    	if (mkdir(working_directory, 511) == -1)
    	{
    		perror("mkdir");
    		return 0;
    	}
    }

#if DEBUG_SHOW_RUNNING_CORE
    int cpucore;
    cpucore = sched_getcpu();

    printf("main running on core %d\n", cpucore);
#endif

    CPU_ZERO(&all_cpu_set);
    for (i = 0; i < 4; i++)
    	CPU_SET(i, &all_cpu_set);

	// Allocate one core for this thread
	/*cores = CPU_ALLOC(1);

	// Bind this thread to core 0.
	if (pthread_setaffinity_np(pthread_self(), sizeof(cores), cores) < 0)
		perror("pthread_setaffinity_np");*/

    pthread_t main_thread;
    pthread_attr_t main_attr;
    struct sched_param main_param;

    if(argc > 1)
    {
        sscanf(argv[1], "%d", &dev);
        printf("using %s\n", argv[1]);

        if (argc > 2)
        {
        	sscanf(argv[2], "%d", &fps);
			printf("fps %s\n", argv[2]);
        }
        else
        {
        	fps = 10;
        }
    }
    else if(argc == 1)
        printf("using default\n");

    else
    {
        printf("usage: capture [dev]\n");
        exit(-1);
    }

    // Set up the scheduler for SCHED_FIFO
    main_thread = pthread_self();
    pthread_attr_init(&main_attr);
    pthread_attr_setinheritsched(&main_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&main_attr, SCHED_FIFO);

    // Make child threads inherit the scheduler of the launching thread
    pthread_attr_init(&capture_attr);
    pthread_attr_setinheritsched(&capture_attr, PTHREAD_INHERIT_SCHED);

    pthread_attr_init(&vid_attr);
    pthread_attr_setinheritsched(&vid_attr, PTHREAD_INHERIT_SCHED);

    // Set priorities
    capture_param.__sched_priority = sched_get_priority_max(SCHED_FIFO);
    main_param.__sched_priority = capture_param.__sched_priority - 1;
    vid_param.__sched_priority = main_param.__sched_priority - 1;

    // Apply scheduling attributes
    sched_setscheduler(getpid(), SCHED_FIFO, &main_param);
    sched_setscheduler(getpid(), SCHED_FIFO, &capture_param);
    sched_setscheduler(getpid(), SCHED_FIFO, &vid_param);
    pthread_attr_setschedparam(&main_attr, &main_param);
    pthread_attr_setschedparam(&capture_attr, &capture_param);
    pthread_attr_setschedparam(&vid_attr, &vid_param);

    print_scheduler();

#if !DEBUG_NO_CAPTURE
    if (!capture_init(dev, string(working_directory)))
    {
    	printf("Failed to open camera on device %d. Timelapse is closing\n", dev);
    	return -1;
    }

    capture_id = pthread_create(&capture_thread, &capture_attr, capture_frame, (void *) 0);
    clock_gettime(CLOCK_REALTIME, &start_time);
    sched_yield();
    while (capture_get_capture_count() < 100 && deadline_misses < 10)
    {
        clock_gettime(CLOCK_REALTIME, &end_time);
        delta_t(&end_time, &start_time, &delta);
        delta_ms = delta.tv_nsec / NSEC_TO_MSEC;
        delta_ms += delta.tv_sec * 1000;

        if (delta_ms >= DEADLINE_MS)
        {
        	clock_gettime(CLOCK_REALTIME, &start_time);
        	sem_post(&capture_sem);
        }
    }

    capture_end_capture(true);
    sem_post(&capture_sem);
    pthread_join(capture_thread, NULL);

    //pthread_kill(capture_thread, 1);

    capture_close(dev);
#else
    capture_set_capture_count(100);
    capture_set_capture_directory(string(working_directory));
#endif

#if !DEBUG_NO_ENCODE
    pthread_create(&vid_thread, &vid_attr, create_video, (void *) 0);
    clock_gettime(CLOCK_REALTIME, &start_time);
    pthread_join(vid_thread, NULL);
    clock_gettime(CLOCK_REALTIME, &end_time);
    delta_t(&end_time, &start_time, &delta);
    printf("Video creation run-time: %ld s %ld nsec\n", delta.tv_sec, delta.tv_nsec);
#endif

	// Free the core this thread is using
	//CPU_FREE(cores);



	pthread_attr_destroy(&main_attr);
	pthread_attr_destroy(&capture_attr);
	pthread_attr_destroy(&vid_attr);

	return 0;
}
