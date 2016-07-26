/******************************************************************************
 * @file capture.cpp
 * @brief Camera capture driver
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

#include <stdio.h>
#include <time.h>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "capture.hpp"
#include "config.h"
#include "utils.hpp"

using namespace cv;
using namespace std;

#define DEFAULT_H_RES		800
#define DEFAULT_V_RES		448

CvCapture *capture;
IplImage* frame;

volatile unsigned long capture_count = 0, total_captures = 0;
volatile unsigned long average_capture_time_nsec = 0;
volatile unsigned long total_capture_time_nsec = 0;

sem_t capture_sem;
pthread_mutex_t capture_in_progress;
pthread_mutex_t capture_complete_mutex;
bool capture_initialized = false;
bool capture_complete = false;

struct binary_semaphore sem_capture_complete;

int capture_init(int dev)
{
	// Initialize the capture
	if ((capture = cvCreateCameraCapture(dev)) == NULL)
	{
		perror("cvCreateCameraCapture");
		return -1;
	}

	// Set the resolution
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, DEFAULT_H_RES);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, DEFAULT_V_RES);

	// Grab the first frame to make sure everything is working and allocate
	// all resources (to save time later)
	if ((frame = cvQueryFrame(capture)) == 0)
	{
		perror("cvQueryFrame");
		return -1;
	}

	sem_init(&capture_sem, 1, 1);

	capture_initialized = true;

	return 1;
}

int capture_close(int dev)
{
	if (capture != NULL)
	{
		cvReleaseCapture(&capture);
	}

	sem_destroy(&capture_sem);

	return 1;
}

void *capture_frame(void *arg)
{
#if DEBUG_CAPTURE_STATS
	struct timespec start_time, end_time, delta;
	clock_gettime(CLOCK_REALTIME, &start_time);
#endif

	int sem_val;

	if (!capture_initialized)
	{
		printf("Capture not initialized!\n");
		pthread_exit(NULL);
		return NULL;
	}

	while (1)
	{
		sem_wait(&capture_sem);

		if ((frame = cvQueryFrame(capture)) == 0)
		{
			perror("cvQueryFrame");
		}
		else
		{
			total_captures++;

#if DEBUG_CAPTURE_STATS
			clock_gettime(CLOCK_REALTIME, &end_time);
			delta_t(&end_time, &start_time, &delta);
			capture_count++;
			total_capture_time_nsec += delta.tv_sec * 1000000000 + delta.tv_nsec;

			if (!(capture_count % 100))
			{
				average_capture_time_nsec = total_capture_time_nsec / capture_count;
				printf("capture_frame: Average capture time over 100 frames: %lu ms\n",
						average_capture_time_nsec / 1000000);
				capture_count = 0;
				total_capture_time_nsec = 0;
				average_capture_time_nsec = 0;
			}
#endif

		}
	}

	pthread_exit(NULL);
	return NULL;
}

long unsigned int capture_get_capture_count(void)
{
	return total_captures;
}
