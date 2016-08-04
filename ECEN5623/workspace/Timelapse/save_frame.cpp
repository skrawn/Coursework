/******************************************************************************
 * @file save_frame.cpp
 * @brief Thread to offload the saving of frames to a file
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

#include <ctime>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <sys/queue.h>
#include <sys/utsname.h>
#include <mqueue.h>
#include <semaphore.h>
#include <error.h>
#include <stdio.h>

#include "save_frame.hpp"

#include "config.h"

#define MAX_FRAMEQ_ITEMS	75

using namespace std;

struct mq_attr frame_mq_attr;
mqd_t frame_mq;

bool initialized = false;
bool last_frame = false;

int capture_length;

string save_directory;

sem_t frame_ready;
pthread_mutex_t frameq_lock;

typedef struct {
	Mat frame;
	int frame_num;
} frameq_item_t;

typedef struct {
	int front;
	int rear;
	int count;
	frameq_item_t frameq_item[MAX_FRAMEQ_ITEMS];
} frameq_t;

frameq_t frameq;

static frameq_item_t *save_frame_pop_frame(void);

void save_frame_init(string save_dir, int cap_len)
{
	frameq.front = 0;
	frameq.rear = -1;
	frameq.count = 0;

	save_directory = save_dir;
	capture_length = cap_len;

	sem_init(&frame_ready, 1, 1);
	pthread_mutex_init(&frameq_lock, NULL);

	initialized = true;
}

void *save_frame(void *arg)
{
	int nbytes, header_length, bytes;
	//int image_properties = CV_IMWRITE_PXM_BINARY;
	const vector<int> image_properties(CV_IMWRITE_PXM_BINARY);
	struct utsname sys_name;
	unsigned int prio;
	char msg_buf[sizeof(IplImage *)];
	unsigned long total_captures = 1;
	frameq_item_t *buf;
	FILE *f, *temp_f;
	char time_string[50] = {0};
	char filename[13] = {0}, full_file_name[256] = {0}, temp_image_file[256] = {0};
	char buffer[64] = {0};
	time_t raw_time;
	filename[0] = 'I'; filename[1] = 'M'; filename[2] = 'G';
	filename[3] = '_';

	strcpy(temp_image_file, save_directory.c_str());
	strcat(temp_image_file, "/temp.PPM");

#if DEBUG_SHOW_RUNNING_CORE
	int cpucore;
	cpucore = sched_getcpu();

	printf("save_frame running on core %d\n", cpucore);
#endif

	while (!last_frame)
	{
		sem_wait(&frame_ready);
		buf = save_frame_pop_frame();
		if (buf != NULL)
		{
			memset(full_file_name, 0, sizeof(full_file_name));

			// Get the current time so we can save it in the file
			time(&raw_time);
			sprintf(time_string, "%s", ctime(&raw_time));

			// Create the filename
			sprintf(&filename[4], "%d.PPM", buf->frame_num);
			strcat(full_file_name, save_directory.c_str());
			strcat(full_file_name, "/");
			strcat(full_file_name, filename);

			// Save the capture to a temporary PPM file
			//if (cvSaveImage(temp_image_file , buf->frame, (const int *) &image_properties) != 1)
			if (!imwrite(string(temp_image_file), buf->frame, image_properties))
			{
				//perror("cvSaveImage");
				printf("save_frame: Failed to write image to file\n");
			}
			else
			{
				// Need to copy the temp image file to the file with the header in it.
				f = fopen(full_file_name, "wb+");
				if (f == NULL)
				{
					perror("fopen");
				}
				temp_f = fopen(temp_image_file, "rb");

				// Copy up to the P6\n part of the image
				fread(&buffer[0], 1, 1, temp_f);
				while(buffer[0] != '\n')
				{
					fwrite(&buffer[0], sizeof(char), 1, f);
					fread(&buffer[0], 1, 1, temp_f);
				}
				fwrite(&buffer[0], sizeof(char), 1, f);

				// Now insert the timestamp
				header_length = sprintf(buffer, "# Created: %s\n", time_string);
				fwrite(buffer, sizeof(char), header_length, f);

				// Insert the platform name
				uname(&sys_name);
				header_length = sprintf(buffer, "# System: %s\n", sys_name.nodename);
				fwrite(buffer, sizeof(char), header_length, f);

				// Write the rest of the image
				while (0 < (bytes = fread(buffer, 1, sizeof(buffer), temp_f)))
					fwrite(buffer, sizeof(char), bytes, f);
				fclose(f);
				fclose(temp_f);

				//cvReleaseImage(&buf->frame);
				//delete(&buf->frame);
			}

			if (capture_length <= (buf->frame_num + 1))
				last_frame = true;
		}
	}

	save_frame_close();

	pthread_exit(NULL);
	return NULL;
}

//int save_frame_save_frame(IplImage *frame, int frame_num)
int save_frame_save_frame(Mat frame, int frame_num)
{
	int index;
	pthread_mutex_lock(&frameq_lock);
	if (frameq.count == MAX_FRAMEQ_ITEMS)
	{
		pthread_mutex_unlock(&frameq_lock);
		printf("save_frame_save_frame: No buffer space available!\n");
		return -1;
	}
	else
	{
		frameq.rear = (frameq.rear + 1) % MAX_FRAMEQ_ITEMS;

		frame.copyTo(frameq.frameq_item[frameq.rear].frame);
		frameq.frameq_item[frameq.rear].frame_num = frame_num;
		frameq.count++;
		pthread_mutex_unlock(&frameq_lock);
		sem_post(&frame_ready);
		return 1;
	}
}

void save_frame_close(void)
{
	//free(frameq.frameq_item);
}

bool save_frame_queue_empty(void)
{
	int count;
	pthread_mutex_lock(&frameq_lock);
	count = frameq.count;
	pthread_mutex_unlock(&frameq_lock);
	return (count == 0);
}

void save_frame_last_frame(bool last)
{
	last_frame = last;
}

bool save_frame_initialized(void)
{
	return initialized;
}

static frameq_item_t *save_frame_pop_frame(void)
{
	frameq_item_t *item;

	if (save_frame_queue_empty())
	{
		return NULL;
	}
	else
	{
		pthread_mutex_lock(&frameq_lock);
		item = &frameq.frameq_item[frameq.front];
		frameq.front = (frameq.front + 1) % MAX_FRAMEQ_ITEMS;
		frameq.count--;
		pthread_mutex_unlock(&frameq_lock);
		return item;
	}
}
