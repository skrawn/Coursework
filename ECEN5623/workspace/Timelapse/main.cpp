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
#include "save_frame.hpp"
#include "utils.hpp"

using namespace std;

#define NSEC_PER_SEC	1000000000

#define NSEC_TO_MSEC_F  1000000.0
#define NSEC_TO_MSEC    1000000
#define SEC_TO_MSEC		1000

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

pthread_t save_frame_thread;
pthread_attr_t save_frame_attr;
struct sched_param save_frame_param;

sem_t frame_signal;

int dev = 0;
int fps = 10;

int main( int argc, char** argv )
{
    struct timespec start_time, end_time, delta;
    struct stat st;
    unsigned long delta_ns, deadline_ns;
    int retval, capture_id, deadline_misses = 0, i = MAX_PATH_LENGTH, num_args,
    		nframes = 100;
    cpu_set_t all_cpu_set, threadcpu;
    char process_path[MAX_PATH_LENGTH] = {0};

    pthread_t main_thread;
	pthread_attr_t main_attr;
	struct sched_param main_param;

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

    if(argc > 1)
    {
    	int arg_num = 1;
    	argc--;
    	while (argc)
    	{
    		if (strcmp(argv[arg_num], "-d") == 0)
    		{
    			argc -= 2;
    			sscanf(argv[++arg_num], "%d", &dev);
    			printf("Using camera device number %d\n", dev);
    			arg_num++;
    		}
    		else if (strcmp(argv[arg_num], "-f") == 0)
    		{
    			// Number of frames
    			argc -= 2;
    			nframes = atoi(argv[++arg_num]);
    			if (nframes < 100 && nframes > 5000)
    			{
    				printf("Error: number of frames must be between 100 and 5000\n");
    				return -1;
    			}
    			printf("Number of frames to capture: %d\n", nframes);
    			arg_num++;
    		}
    		else if (strcmp(argv[arg_num], "-fr") == 0)
    		{
    			argc -= 2;
    			fps = atoi(argv[++arg_num]);
    			if (fps <= 0 && fps > 60)
    			{
    				printf("Error: framerate must be between 1 and 60 fps\n");
    				return -1;
    			}
    			printf("Framerate: %d fps\n", fps);
    			arg_num++;
    		}
    		else
    		{
    			printf("Error: Invalid input.\n");
    			return -1;
    		}
    	}
    }
    else if(argc == 1)
        printf("using defaults: camera device 0, 100 frames @ 10Hz\n");

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
    pthread_attr_setinheritsched(&capture_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&capture_attr, SCHED_FIFO);

    pthread_attr_init(&vid_attr);
    pthread_attr_setinheritsched(&vid_attr, PTHREAD_INHERIT_SCHED);

    pthread_attr_init(&save_frame_attr);
    pthread_attr_setinheritsched(&save_frame_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&save_frame_attr, SCHED_FIFO);

    // Set priorities
    capture_param.__sched_priority = sched_get_priority_max(SCHED_FIFO);
    save_frame_param.__sched_priority = capture_param.__sched_priority - 1;
    main_param.__sched_priority = save_frame_param.__sched_priority - 1;
    vid_param.__sched_priority = main_param.__sched_priority - 1;

    // Apply scheduling attributes
    sched_setscheduler(getpid(), SCHED_FIFO, &main_param);
    sched_setscheduler(getpid(), SCHED_FIFO, &capture_param);
    sched_setscheduler(getpid(), SCHED_FIFO, &vid_param);
    sched_setscheduler(getpid(), SCHED_FIFO, &save_frame_param);
    pthread_attr_setschedparam(&main_attr, &main_param);
    pthread_attr_setschedparam(&capture_attr, &capture_param);
    pthread_attr_setschedparam(&vid_attr, &vid_param);
    pthread_attr_setschedparam(&save_frame_attr, &save_frame_param);

    print_scheduler();

#if !DEBUG_NO_CAPTURE
    if (!capture_init(dev, string(working_directory), nframes))
    {
    	printf("Failed to open camera on device %d. Timelapse is closing\n", dev);
    	return -1;
    }

    capture_id = pthread_create(&capture_thread, &capture_attr, capture_frame, (void *) 0);
    sched_yield();

    save_frame_init(working_directory, nframes);
    pthread_create(&save_frame_thread, &save_frame_attr, save_frame, (void *) 0);
    sched_yield();

    deadline_ns = 1000000000 / fps;

    clock_gettime(CLOCK_REALTIME, &start_time);
    while (capture_get_capture_count() < nframes)
    {
        clock_gettime(CLOCK_REALTIME, &end_time);
        delta_t(&end_time, &start_time, &delta);
        delta_ns = delta.tv_nsec;
        delta_ns += delta.tv_sec * 1000000000;

        if (delta_ns >= deadline_ns)
        {
        	// Check that the capture thread is waiting at the semaphore
        	sem_getvalue(&capture_sem, &retval);
        	clock_gettime(CLOCK_REALTIME, &start_time);
        	if (retval > 0)
        	{
        		deadline_misses++;
        	}
        	else
        	{
        		/*if (capture_get_capture_count() >= (nframes - 1) )
        			save_frame_last_frame(true);*/
        		sem_post(&capture_sem);
        	}
        }
        sched_yield();
    }

    capture_end_capture(true);
    sem_post(&capture_sem);
    pthread_join(capture_thread, NULL);

    capture_close(dev);

    pthread_join(save_frame_thread, NULL);

    if (deadline_misses > 0)
    	printf("Capture deadline misses: %d\n", deadline_misses);

    capture_set_capture_count(nframes);

#else
    capture_set_capture_count(nframes);
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

	pthread_attr_destroy(&main_attr);
	pthread_attr_destroy(&capture_attr);
	pthread_attr_destroy(&vid_attr);

	return 0;
}
