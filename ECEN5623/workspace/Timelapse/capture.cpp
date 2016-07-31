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

#include <ctime>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "capture.hpp"
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

string capture_dir = "";

static void *save_frame(void *arg);

int capture_init(int dev, string cap_dir)
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

	capture_dir = cap_dir;

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
	volatile unsigned long max_runtime = 0, min_runtime = ULONG_MAX, capture_time;
#endif

#if DEBUG_SHOW_RUNNING_CORE
	int cpucore;
	cpucore = sched_getcpu();

	printf("capture_frame running on core %d\n", cpucore);
#endif

	int image_properties = CV_IMWRITE_PXM_BINARY;
	int sem_val, header_length, bytes;
	time_t raw_time;
	FILE *f, *temp_f;
	char time_string[50] = {0};
	char filename[10] = {0}, full_file_name[256] = {0}, temp_image_file[256] = {0};
	char buffer[64] = {0};
	filename[0] = 'I'; filename[1] = 'M'; filename[2] = 'G';
	filename[3] = '_';

	strcpy(temp_image_file, capture_dir.c_str());
	strcat(temp_image_file, "/temp.PPM");

	if (!capture_initialized)
	{
		printf("Capture not initialized!\n");
		pthread_exit(NULL);
		return NULL;
	}

	while (1)
	{
		sem_wait(&capture_sem);
		clock_gettime(CLOCK_REALTIME, &start_time);

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
			capture_time = delta.tv_sec * 1000000000 + delta.tv_nsec;
			if (max_runtime < capture_time)
				max_runtime = capture_time;
			if (min_runtime > capture_time)
				min_runtime = capture_time;
			total_capture_time_nsec += capture_time;

			if (!(capture_count % 100))
			{
				average_capture_time_nsec = total_capture_time_nsec / capture_count;
				printf("capture_frame: Average capture time over 100 frames: %lu ms - Max: %lu - Min: %lu\n",
						average_capture_time_nsec / 1000000, max_runtime / 1000000, min_runtime / 1000000);
				capture_count = 0;
				total_capture_time_nsec = 0;
				average_capture_time_nsec = 0;
				max_runtime = 0;
				min_runtime = ULONG_MAX;
			}
#endif
			// Save the capture to the directory
			if (!capture_dir.empty())
			{
				memset(full_file_name, 0, sizeof(full_file_name));

				// Get the current time so we can save it in the file
				time(&raw_time);
				sprintf(time_string, "%s", ctime(&raw_time));

				// Create the filename
				sprintf(&filename[4], "%lu.PPM", total_captures);
				strcat(full_file_name, capture_dir.c_str());
				strcat(full_file_name, "/");
				strcat(full_file_name, filename);

				// Save the capture to a temporary PPM file
				if (cvSaveImage(temp_image_file , frame, (const int *) &image_properties) != 1)
				{
					perror("cvSaveImage");
				}
				else
				{
					// Need to copy the temp image file to the file with the header in it.
					f = fopen(full_file_name, "wb");
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

					// Write the rest of the image
					while (0 < (bytes = fread(buffer, 1, sizeof(buffer), temp_f)))
						fwrite(buffer, sizeof(char), bytes, f);
					fclose(f);
					fclose(temp_f);
				}
			}
		}
	}

	pthread_exit(NULL);
	return NULL;
}

void *create_video(void *arg)
{
	//AVCodec *codec;
	//AVCodecContext *c = NULL;
	//AVFrame *picture;

#if DEBUG_SHOW_RUNNING_CORE
	int cpucore;
	cpucore = sched_getcpu();

	printf("create_video running on core %d\n", cpucore);
#endif

	// Find the MPEG4 video encoder
	/*codec = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
	if (!codec)
	{
		printf("MPEG4 Codec not found!\n");
		pthread_exit(NULL);
	}

	c = avcodec_alloc_context3(codec);
	//picture = avcodec_alloc_frame();

	// Sample parameters
	c->bit_rate = 400000;
	c->width = 800;
	c->height = 448;
	c->time_base = (AVRational) {1, 25};
	c->gop_size = 10;	// emit one intra frame every ten frames
	c->max_b_frames = 1;
	//c->pix_fmt = AV_PIX_FMT_YUV420P;
	c->pix_fmt = AV_PIX_FMT_RGB24;

	// Open the codec
	if (avcodec_open2(c, codec, NULL) < 0) {
		printf("Could not open codec!\n");
		pthread_exit(NULL);
	}


	// Open the output file
	//f = fopen("")*/

	pthread_exit(NULL);
	return NULL;
}

long unsigned int capture_get_capture_count(void)
{
	return total_captures;
}

void capture_set_capture_directory(string directory)
{
	capture_dir = directory;
}

static void *save_frame(void *arg)
{

	pthread_exit(NULL);
	return NULL;
}
