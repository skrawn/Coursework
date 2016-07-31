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

#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/error.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>

}

using namespace cv;
using namespace std;

#define DEFAULT_H_RES				800
#define DEFAULT_V_RES				448

// Video output parameters
#define DEFAULT_BIT_RATE			1500000	// bps
#define DEFAULT_GOP_SIZE			12
//#define DEFAULT_PIX_FMT			AV_PIX_FMT_RGB24
#define DEFAULT_PIX_FMT 			AV_PIX_FMT_YUV420P
#define DEFAULT_FRAME_RATE			20		// frames/sec
#define DEFAULT_VIDEO_OUTBUF_SIZE	200000;
#define DEFAULT_DURATION			10.0	// seconds

#define SCALE_FLAGS 				SWS_BICUBIC

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
bool end_capture = false;

struct binary_semaphore sem_capture_complete;

string capture_dir = "";

typedef struct OutputStream {
	AVStream *st;
	AVCodecContext *enc;

	// pts of the next frame that will be generated
	int64_t next_pts;
	int samples_count;

	AVFrame *frame;
	AVFrame *tmp_frame;

	float t, tincr, tincr2;

	struct SwsContext *sws_ctx;
	struct SwrContext *swr_ctx;
} OutputStream;

static void open_video(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg);
static void add_stream(OutputStream *ost, AVFormatContext *oc, AVCodec **codec, enum AVCodecID codec_id);
static int write_frame(AVFormatContext *fmt_ctx, const AVRational *time_base, AVStream *st, AVPacket *pkt);
static int write_video_frame(AVFormatContext *oc, OutputStream *ost);
static AVFrame *get_video_frame(OutputStream *ost);
static AVFrame *alloc_picture(enum AVPixelFormat pix_fmt, int width, int height);
static void close_stream(AVFormatContext *oc, OutputStream *ost);
static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt);
static void fill_yuv_image(AVFrame *pict, int frame_index,
                           int width, int height);
static void *save_frame(void *arg);

#ifdef  __cplusplus
// Some fixes for the inline functions av_err2str and av_ts2str.

static const string av_make_error_string(int errnum)
{
	char errbuf[AV_ERROR_MAX_STRING_SIZE];
	av_strerror(errnum, errbuf, AV_ERROR_MAX_STRING_SIZE);
	return (string)errbuf;
}

#undef av_err2str
#define av_err2str(errnum) av_make_error_string(errnum).c_str()

static const string av_ts_make_string(int64_t ts)
{
	char buf[AV_TS_MAX_STRING_SIZE];
	if (ts == AV_NOPTS_VALUE) snprintf(buf, AV_TS_MAX_STRING_SIZE, "NOPTS");
	else					  snprintf(buf, AV_TS_MAX_STRING_SIZE, "%"PRId64, ts);
	return (string) buf;
}

#undef av_ts2str
#define av_ts2str(ts) av_ts_make_string(ts).c_str()

static const string av_ts_make_time_string(int64_t ts, AVRational *tb)
{
	char buf[AV_TS_MAX_STRING_SIZE];
	if (ts == AV_NOPTS_VALUE) snprintf(buf, AV_TS_MAX_STRING_SIZE, "NOPTS");
	else					  snprintf(buf, AV_TS_MAX_STRING_SIZE, "%.6g", av_q2d(*tb) * ts);
	return (string) buf;
}

#undef av_ts2timestr
#define av_ts2timestr(ts, tb) av_ts_make_time_string(ts, tb).c_str()

#endif // __cplusplus

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
	OutputStream video_st = {0}, audio_st = {0};
	AVCodec *video_codec;
	AVOutputFormat *fmt;
	AVFormatContext *oc = NULL;
	AVDictionary *opt = NULL;
	double video_pts;
	int i, ret, have_video = 0, have_audio = 0, encode_video = 0, encode_audio = 0;

	char video_filename[] = {"Capture.mp4\0"};

#if DEBUG_SHOW_RUNNING_CORE
	int cpucore;
	cpucore = sched_getcpu();

	printf("create_video running on core %d\n", cpucore);
#endif

	// Initialize libavcodec and register all codecs and formats
	av_register_all();

	// Allocate the output media context
	avformat_alloc_output_context2(&oc, NULL, NULL, video_filename);
	if (!oc)
	{
		printf("create_video: Could not deduce output format from file extension. Using MPEG.\n");
		avformat_alloc_output_context2(&oc, NULL, "mpeg", video_filename);
	}

	fmt = oc->oformat;

	// Add the video streams using the default format codecs and initialize the codecs
	if (fmt->video_codec != AV_CODEC_ID_NONE)
	{
		add_stream(&video_st, oc, &video_codec, fmt->video_codec);
		have_video = 1;
		encode_video = 1;
	}

	// Open the video codec
	if (have_video)
		open_video(oc, video_codec, &video_st, opt);

	av_dump_format(oc, 0, video_filename, 1);

	// Open the output file, if needed
	if (!(fmt->flags & AVFMT_NOFILE))
	{
		ret = avio_open(&oc->pb, video_filename, AVIO_FLAG_WRITE);
		if (ret < 0)
		{
			printf("create_video: Could not open '%s': %s\n", video_filename,
					av_err2str(ret));
			pthread_exit(NULL);
		}
	}

	// write the stream header, if any
	ret = avformat_write_header(oc, &opt);
	if (ret < 0)
	{
		printf("create_video: Error occurred when opening output file: %s\n",
				av_err2str(ret));
		pthread_exit(NULL);
	}

	while (encode_video)
	{
		// Select the stream to encode
		if (encode_video &&
			(!encode_audio || av_compare_ts(video_st.next_pts, video_st.enc->time_base,
										    audio_st.next_pts, audio_st.enc->time_base) <= 0))
		{
			encode_video = !write_video_frame(oc, &video_st);
		}
	}

	// Write the trailer, if any. The trailer must be written before closing the
	// CodecContexts open when the header was written; otherwise av_write_trailer()
	// may try to use memory that was freed on av_codec_close().
	av_write_trailer(oc);

	// Close each codec
	if (have_video)
		close_stream(oc, &video_st);
	if (have_audio)
		close_stream(oc, &audio_st);

	if (!(fmt->flags & AVFMT_NOFILE))
		// Close the output file
		avio_closep(&oc->pb);

	// Free the stream
	avformat_free_context(oc);

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

static void open_video(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg)
{
    int ret;
    AVCodecContext *c = ost->enc;
    AVDictionary *opt = NULL;

    av_dict_copy(&opt, opt_arg, 0);

    /* open the codec */
    ret = avcodec_open2(c, codec, &opt);
    av_dict_free(&opt);
    if (ret < 0) {
        printf("open_video: Could not open video codec: %s\n", av_err2str(ret));
        exit(1);
    }

    /* allocate and init a re-usable frame */
    ost->frame = alloc_picture(c->pix_fmt, c->width, c->height);
    if (!ost->frame) {
        printf("open_video: Could not allocate video frame\n");
        exit(1);
    }

    /* If the output format is not YUV420P, then a temporary YUV420P
     * picture is needed too. It is then converted to the required
     * output format. */
    ost->tmp_frame = NULL;
    if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
        ost->tmp_frame = alloc_picture(AV_PIX_FMT_YUV420P, c->width, c->height);
        if (!ost->tmp_frame) {
            printf("open_video: Could not allocate temporary picture\n");
            exit(1);
        }
    }

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(ost->st->codecpar, c);
    if (ret < 0) {
        printf("open_video: Could not copy the stream parameters\n");
        exit(1);
    }
}

static void add_stream(OutputStream *ost, AVFormatContext *oc,
                       AVCodec **codec,
                       enum AVCodecID codec_id)
{
    AVCodecContext *c;
    int i;

    /* find the encoder */
    *codec = avcodec_find_encoder(codec_id);
    if (!(*codec)) {
        printf("add_stream: Could not find encoder for '%s'\n",
                avcodec_get_name(codec_id));
        exit(1);
    }

    ost->st = avformat_new_stream(oc, NULL);
    if (!ost->st) {
        printf("add_stream: Could not allocate stream\n");
        exit(1);
    }
    ost->st->id = oc->nb_streams-1;
    c = avcodec_alloc_context3(*codec);
    if (!c) {
        printf("add_stream: Could not alloc an encoding context\n");
        exit(1);
    }
    ost->enc = c;

    switch ((*codec)->type) {
    case AVMEDIA_TYPE_AUDIO:
        c->sample_fmt  = (*codec)->sample_fmts ?
            (*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
        c->bit_rate    = 64000;
        c->sample_rate = 44100;
        if ((*codec)->supported_samplerates) {
            c->sample_rate = (*codec)->supported_samplerates[0];
            for (i = 0; (*codec)->supported_samplerates[i]; i++) {
                if ((*codec)->supported_samplerates[i] == 44100)
                    c->sample_rate = 44100;
            }
        }
        c->channels        = av_get_channel_layout_nb_channels(c->channel_layout);
        c->channel_layout = AV_CH_LAYOUT_STEREO;
        if ((*codec)->channel_layouts) {
            c->channel_layout = (*codec)->channel_layouts[0];
            for (i = 0; (*codec)->channel_layouts[i]; i++) {
                if ((*codec)->channel_layouts[i] == AV_CH_LAYOUT_STEREO)
                    c->channel_layout = AV_CH_LAYOUT_STEREO;
            }
        }
        c->channels        = av_get_channel_layout_nb_channels(c->channel_layout);
        ost->st->time_base = (AVRational){ 1, c->sample_rate };
        break;

    case AVMEDIA_TYPE_VIDEO:
        c->codec_id = codec_id;

        c->bit_rate = DEFAULT_BIT_RATE;
        /* Resolution must be a multiple of two. */
        c->width    = DEFAULT_H_RES;
        c->height   = DEFAULT_V_RES;
        /* timebase: This is the fundamental unit of time (in seconds) in terms
         * of which frame timestamps are represented. For fixed-fps content,
         * timebase should be 1/framerate and timestamp increments should be
         * identical to 1. */
        ost->st->time_base = (AVRational){ 1, DEFAULT_FRAME_RATE };
        c->time_base       = ost->st->time_base;

        c->gop_size      = DEFAULT_GOP_SIZE; /* emit one intra frame every twelve frames at most */
        c->pix_fmt       = DEFAULT_PIX_FMT;
        if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
            /* just for testing, we also add B-frames */
            c->max_b_frames = 2;
        }
        if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
            /* Needed to avoid using macroblocks in which some coeffs overflow.
             * This does not happen with normal video, it just happens here as
             * the motion of the chroma plane does not match the luma plane. */
            c->mb_decision = 2;
        }
    break;

    default:
        break;
    }

    /* Some formats want stream headers to be separate. */
    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

static int write_frame(AVFormatContext *fmt_ctx, const AVRational *time_base, AVStream *st, AVPacket *pkt)
{
    /* rescale output packet timestamp values from codec to stream timebase */
    av_packet_rescale_ts(pkt, *time_base, st->time_base);
    pkt->stream_index = st->index;

    /* Write the compressed frame to the media file. */
    log_packet(fmt_ctx, pkt);
    return av_interleaved_write_frame(fmt_ctx, pkt);
}

static int write_video_frame(AVFormatContext *oc, OutputStream *ost)
{
    int ret;
    AVCodecContext *c;
    AVFrame *frame;
    int got_packet = 0;
    AVPacket pkt = { 0 };

    c = ost->enc;

    frame = get_video_frame(ost);

    av_init_packet(&pkt);

    /* encode the image */
    ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);
    if (ret < 0) {
        printf("write_video_frame: Error encoding video frame: %s\n", av_err2str(ret));
        exit(1);
    }

    if (got_packet) {
        ret = write_frame(oc, &c->time_base, ost->st, &pkt);
    } else {
        ret = 0;
    }

    if (ret < 0) {
        printf("write_video_frame: Error while writing video frame: %s\n", av_err2str(ret));
        exit(1);
    }

    return (frame || got_packet) ? 0 : 1;
}

static AVFrame *get_video_frame(OutputStream *ost)
{
    AVCodecContext *c = ost->enc;

    /* check if we want to generate more frames */
    if (av_compare_ts(ost->next_pts, c->time_base,
                      DEFAULT_DURATION, (AVRational){ 1, 1 }) >= 0)
        return NULL;

    if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
        /* as we only generate a YUV420P picture, we must convert it
         * to the codec pixel format if needed */
        if (!ost->sws_ctx) {
            ost->sws_ctx = sws_getContext(c->width, c->height,
                                          AV_PIX_FMT_YUV420P,
                                          c->width, c->height,
                                          c->pix_fmt,
                                          SCALE_FLAGS, NULL, NULL, NULL);
            if (!ost->sws_ctx) {
                fprintf(stderr,
                        "Could not initialize the conversion context\n");
                exit(1);
            }
        }
        // Grab our frames here
        fill_yuv_image(ost->tmp_frame, ost->next_pts, c->width, c->height);
        sws_scale(ost->sws_ctx,
                  (const uint8_t * const *)ost->tmp_frame->data, ost->tmp_frame->linesize,
                  0, c->height, ost->frame->data, ost->frame->linesize);
    } else {
        fill_yuv_image(ost->frame, ost->next_pts, c->width, c->height);
    }

    ost->frame->pts = ost->next_pts++;

    return ost->frame;
}

static AVFrame *alloc_picture(enum AVPixelFormat pix_fmt, int width, int height)
{
    AVFrame *picture;
    int ret;

    picture = av_frame_alloc();
    if (!picture)
        return NULL;

    picture->format = pix_fmt;
    picture->width  = width;
    picture->height = height;

    /* allocate the buffers for the frame data */
    ret = av_frame_get_buffer(picture, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate frame data.\n");
        exit(1);
    }

    return picture;
}

static void close_stream(AVFormatContext *oc, OutputStream *ost)
{
    avcodec_free_context(&ost->enc);
    av_frame_free(&ost->frame);
    av_frame_free(&ost->tmp_frame);
    sws_freeContext(ost->sws_ctx);
    swr_free(&ost->swr_ctx);
}

static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{
    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

    printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
           av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
           av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
           av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
           pkt->stream_index);
}

/* Prepare a dummy image. */
static void fill_yuv_image(AVFrame *pict, int frame_index,
                           int width, int height)
{
    int x, y, i, ret;

    /* when we pass a frame to the encoder, it may keep a reference to it
     * internally;
     * make sure we do not overwrite it here
     */
    ret = av_frame_make_writable(pict);
    if (ret < 0)
        exit(1);

    i = frame_index;

    /* Y */
    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++)
            pict->data[0][y * pict->linesize[0] + x] = x + y + i * 3;

    /* Cb and Cr */
    for (y = 0; y < height / 2; y++) {
        for (x = 0; x < width / 2; x++) {
            pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
            pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
        }
    }
}

static void *save_frame(void *arg)
{

	pthread_exit(NULL);
	return NULL;
}
