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

extern "C" {

#include <libavdevice/avdevice.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>

}

using namespace cv;
using namespace std;

#define DEFAULT_H_RES				800
#define DEFAULT_V_RES				448

// Video output parameters
#define DEFAULT_BIT_RATE			1500000
#define DEFAULT_GOP_SIZE			12
//#define DEFAULT_PIX_FMT			AV_PIX_FMT_RGB24
#define DEFAULT_PIX_FMT 			AV_PIX_FMT_YUV420P
#define DEFAULT_FRAME_RATE			20
#define DEFAULT_VIDEO_OUTBUF_SIZE	200000;

CvCapture *capture;
IplImage* frame;

AVFrame *picture, *tmp_picture;
uint8_t *video_outbuf;
int frame_count, video_outbuf_size;

volatile unsigned long capture_count = 0, total_captures = 0;
volatile unsigned long average_capture_time_nsec = 0;
volatile unsigned long total_capture_time_nsec = 0;

sem_t capture_sem;
pthread_mutex_t capture_in_progress;
pthread_mutex_t capture_complete_mutex;
bool capture_initialized = false;
bool capture_complete = false;
bool end_capture = false;

struct binary_semaphore sem_capture_complete;

string capture_dir = "";

static int sws_flags = SWS_BICUBIC;

static AVStream *add_video_stream(AVFormatContext *oc, AVCodecID codec_id);
static void open_video(AVFormatContext *oc, AVStream *st);
static void close_video(AVFormatContext *oc, AVStream *st);
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

	while (!end_capture)
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
	AVCodec *codec;
	AVOutputFormat *fmt;
	AVFormatContext *oc = NULL;
	AVStream *video_st;
	AVFrame *picture;
	double video_pts;
	int i;
	char video_filename[] = {"Capture.mp4\0"};

#if DEBUG_SHOW_RUNNING_CORE
	int cpucore;
	cpucore = sched_getcpu();

	printf("create_video running on core %d\n", cpucore);
#endif

	// Initialize libavcodec and register all codecs and formats
	av_register_all();

	// Find the MPEG4 video encoder
	codec = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
	if (!codec)
	{
		printf("MPEG4 Codec not found!\n");
		pthread_exit(NULL);
	}

	// Selects the output format based on the filename
	fmt = av_guess_format(NULL, video_filename, NULL);
	if (!fmt)
	{
		printf("Could not determine output format from file extension: using MPEG.\n");
		fmt = av_guess_format("mpeg", NULL, NULL);
	}
	if (!fmt)
	{
		printf("Could not find suitable output format\n");
		pthread_exit(NULL);
	}

	// Allocate the output media context
	oc = avformat_alloc_context();
	if (!oc) {
		printf("Could not allocate output media context\n");
		pthread_exit(NULL);
	}
	oc->oformat = fmt;
	snprintf(oc->filename, sizeof(oc->filename), "%s", video_filename);

	// Add the video stream and initialize the codec
	video_st = NULL;
	if (fmt->video_codec != AV_CODEC_ID_NONE)
	{
		if ((video_st = add_video_stream(oc, fmt->video_codec)) != NULL)
			pthread_exit(NULL);
	}

	// Set the output parameters - must be done even if no parameters
	//if (av_set_parameters(oc, NULL) < 0)
	/*if (v4l2_set_parameters(oc) < 0)
	{
		printf("Invalid output format parameters\n");
		pthread_exit(NULL);
	}*/

	av_dump_format(oc, 0, video_filename, 1);

	// Open the video codecs and allocate the necessary encode buffers
	if (video_st)
		open_video(oc, video_st);

	// Open the output file, if needed
	if (!(fmt->flags & AVFMT_NOFILE))
	{
		if (url_fopen(&oc->pb, video_filename, URL_WRONLY) < 0)
		{
			printf("Could not open %s\n", filename);
			pthread_exit(NULL);
		}
	}

	// Write the stream header, if any
	av_write_header(oc);

	for (;;)
	{
		if (video_st)
			video_pts = (double)video_st->pts.val * video_st->time_base.num / video_st->time_base.den;
		else
			video_pts = 0.0;

		if (!video_st || video_pts >= STREAM_DURATION)
			break;

		// Write interleaved video frames
		write_video_frame(oc, video_st);
	}

	// Write the trailer, if any. The trailer must be written before closing the
	// CodecContexts open when the header was written. Otherwise, write_trailer
	// may try to use memory that was freed on av_codec_close()
	av_write_trailer(oc);

	// Close video codec
	if (video_st)
		close_video(oc, video_st);

	// Free the stream
	for (i = 0; i < oc->nb_streams; i++)
	{
		av_freep(&oc->streams[i]->codec);
		av_freep(&oc->streams[i]);
	}

	if (!(fmt->flags & AVFMT_NOFILE))
	{
		// Close the output file
		url_fclose(oc->pb);
	}

	// Free the stream
	av_free(oc);

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

void capture_end_capture(bool end)
{
	end_capture = end;
}

static AVStream *add_video_stream(AVFormatContext *oc, AVCodecID codec_id);
{
	AVCodecContext *c;
	AVStream *st;

	st = av_new_stream(oc, 0);
	if (!st)
	{
		printf("add_video_stream: Could not allocate stream!\n");
		return NULL;
	}

	c = st->codec;
	c->codec_id = codec_id;
	c->codec_type = AVMEDIA_TYPE_VIDEO;

	// Put sample parameters
	c->bit_rate = DEFAULT_BIT_RATE;

	// Resolutions must be multiples of 2
	c->width = DEFAULT_HRES;
	c->height = DEFAULT_VRES;

	// Fundamental unit of time in terms of which frame timestamps are represented.
	// For fixed-fps context, timebase should be 1/framerate and timestamp increments
	// should be identically 1.
	c->time_base.den = STREAM_FRAME_RATE;
	c->time_base.num = 1;

	// Emit one intra frame every 12 frames at most
	c->gop_size = DEFAULT_GOP_SIZE;
	c->pix_fmt = STREAM_PIX_FMT;

	if (c->codec_id == CODEC_ID_MPEG2VIDEO)
	{
		// Just for testing, also add B frames
		c->max_b_frames = 2;
	}
	if (c->codec_id == CODEC_ID_MPEG1VIDEO)
	{
		// Needed to avoid macroblocks in which some coeffs overflow.
		// This does not happen with normal video, it just happens here
		// as the motion of the chroma plane does not match the luma plane.
		c->mb_decision = 2;
	}
	// Some formats want stream headers to be separate
	if (oc->oformat->flags & AVFMT_GLOBALHEADER)
		c->flags |= CODEC_FLAG_GLOBAL_HEADER;

	return st;
}

static void open_video(AVFormatContext *oc, AVStream *st)
{
	AVCodec *codec;
	AVCodecContext *c;

	c = st->codec;

	// Find the video encoder
	codec = avcodec_find_encoder(c->codec_id);
	if (!codec)
	{
		printf("open_video: Codec not found\n");
		return;
	}

	// Open the codec
	if (avcodec_open(c, codec) < 0)
	{
		printf("open_video: Could not open codec\n");
		return;
	}

	video_outbuf = NULL;
	if (!(oc->oformat->flags & AVFMT_RAWPICTURE))
	{
		// Allocate output buffer. Buffers passed into lav* can be allocated any way you prefer.
		// as long as they're aligned enough for the architecture, and they're freed appropriately
		// (such as using av_free for buffers and allocated with av_malloc)
		video_outbuf_size = DEFAULT_VIDEO_OUTBUF_SIZE;
		video_outbuf = av_malloc(video_outbuf_size);
	}

	// Allocate the encoded raw picture
	picture = alloc_picture(c->pix_fmt, c->width, c->height);
	if (!picture)
	{
		printf("open_video: Could not allocate picture\n");
		return;
	}

	// If the output format is not YUV420P, then a temporary YUV420P picture
	// is needed too. It is then converted to the required output format.
	tmp_picture = NULL;
	if (c->pix_fmt != AV_PIX_FMT_YUV420P)
	{
		tmp_picture = alloc_picture(AV_PIX_FMT_YUV420P, c->width, c->height);
		if (!tmp_picture)
		{
			printf("open_video: Could not allocate temporary picture\n");
			return;
		}
	}
}

static void write_video_frame(AVFormatContext *oc, AVStream *st)
{
	int out_size, ret;
	AVCodecContext *c;
	static struct SwsContext *img_convert_ctx;

	c = st->codec;

	if (frame_count >= STREAM_NB_FRAMES)
	{
		// No more frame to compress. The codec has a latency of a few frames
		// if using B frames, so we get the last frames by passing the same
		// picture again.
	}
	else
	{
		if (c->pix_fmt != AV_PIX_FMT_YUV420P)
		{
			// As we only generate a YUV420P picture, we must convert it to the
			// codec pixel format if needed
			if (img_convert_ctx == NULL)
			{
				img_convert_ctx = sws_getContext(c->width, c->height,
												AV_PIX_FMT_YUV420P,
												c->width, c->height,
												c->pix_fmt, sws_flags,
												NULL, NULL, NULL);
				if (img_convert_ctx == NULL)
				{
					printf("write_video_frame: Cannot initialize the conversion context\n");
					return;
				}
			}
			// These fill_yuv_image are used to generate dummy images. This is where the
			// captured frames will need to be loaded.
			//fill_yuv_image(tmp_picture, frame_count, c->width, c->height);
			sws_scale(img_convert_ctx, tmp_picture->data, tmp_picture->linesize,
					0, c->height, picture->data, picture->linesize);
		}
		else
		{
			//fill_yuv_image(picture, frame_count, c->width, c->height);
		}
	}

	if (oc->oformat->flags & AVFMT_RAWPICTURE)
	{
		// Raw video case. The API will change slightly in the near future for that.
		AVPacket pkt;
		av_init_packet(&pkt);

		pkt.flags |= AV_PKT_FLAG_KEY;
		pkt.stream_index = st->index;
		pkt.data = (uint8_t *) picture;
		pkt.size = sizeof(AVPicture);

		ret = av_interleaved_write_frame(oc, &pkt);
	}
	else
	{
		// Encode the image
		out_size = avcodec_encode_video(c, video_outbuf, video_outbuf_size, picture);

		// If zero size, it means the image was buffered
		if (out_size > 0)
		{
			AVPacket pkt;
			av_init_packet(&pkt);

			if (c->coded_frame->pts != AV_NOPTS_VALUE)
				pkt.pts = av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
			if (c->coded_frame->key_frame)
				pkt.flags |= AV_PKT_FLAG_KEY;

			pkt.stream_index = st->index;
			pkt.data = video_outbuf;
			pkt.size = out_size;

			// Write the compressed frame in the media file
			ret = av_interleaved_write_frame(oc, &pkt);
		}
		else
		{
			ret = 0;
		}
	}

	if (ret != 0)
	{
		printf("write_video_frame: Error while writing video frame\n");
	}

	frame_count++;
}

static void close_video(AVFormatContext *oc, AVStream *st)
{
	avcodec_close(st->codec);
	av_free(picture->data[0]);
	av_free(picture);
	if (tmp_picture)
	{
		av_free(tmp_picture->data[0]);
		av_free(tmp_picture);
	}
	av_free(video_outbuf);
}

static void *save_frame(void *arg)
{

	pthread_exit(NULL);
	return NULL;
}
