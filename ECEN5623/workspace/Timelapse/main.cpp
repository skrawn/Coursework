/*
 *
 *  Example by Sam Siewert
 *
 */
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

#define N_RESOLUTIONS	5
#define N_FRAMES        100
#define NSEC_PER_SEC	1000000000

#define NSEC_TO_MSEC_F  1000000.0
#define NSEC_TO_MSEC    1000000
#define SEC_TO_MSEC	1000

#define OK				0
#define ERROR			1

#define N_TRANSFORMS    4
#define CANNY           0
#define SOBEL           1
#define HOUGH           2
#define CAPTURE_ONLY	3

#define DEADLINE_MS     100 // ms

char timg_window_name[] = "Frame Rate Tests";

int lowThreshold=0;
int const max_lowThreshold = 100;
int kernel_size = 3;
int edgeThresh = 1;
int ratio = 3;
Mat canny_frame, cdst, timg_gray, timg_grad;

IplImage* frame;

pthread_t frame_thread;
pthread_attr_t frame_attr;
struct sched_param frame_param;

sem_t frame_signal;
sem_t capture_complete;
bool test_complete = false;

unsigned int res_index = 0;
unsigned int transform_index = 0;
unsigned int frame_count = 0;

int dev=0;

double capture_stats[N_RESOLUTIONS][4][7] = {0};

unsigned int resolutions[N_RESOLUTIONS][2] = {
	{128, 72},
	{384, 216},
	{640, 360},
	{768, 432},
	{800, 448}
};

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

void print_scheduler(void)
{
   int schedType;

   schedType = sched_getscheduler(getpid());

   switch(schedType)
   {
     case SCHED_FIFO:
           printf("Pthread Policy is SCHED_FIFO\n");
           break;
     case SCHED_OTHER:
           printf("Pthread Policy is SCHED_OTHER\n");
       break;
     case SCHED_RR:
           printf("Pthread Policy is SCHED_OTHER\n");
           break;
     default:
       printf("Pthread Policy is UNKNOWN\n");
   }
}

void CannyThreshold(int, void*)
{
    Mat mat_frame(frame);

    cvtColor(mat_frame, timg_gray, CV_RGB2GRAY);

    /// Reduce noise with a kernel 3x3
    blur( timg_gray, canny_frame, Size(3,3) );

    /// Canny detector
    Canny( canny_frame, canny_frame, lowThreshold, lowThreshold*ratio, kernel_size );

    /// Using Canny's output as a mask, we display our result
    //timg_grad = Scalar::all(0);

   // mat_frame.copyTo( timg_grad, canny_frame);

   // imshow( timg_window_name, timg_grad );

}

void HoughTransform(void)
{
    Mat mat_frame(frame);
    vector<Vec4i> lines;

    Canny(mat_frame, canny_frame, 50, 200, 3);

    HoughLinesP(canny_frame, lines, 1, CV_PI/180, 50, 50, 10);

    for (size_t i = 0; i < lines.size(); i++)
    {
    	Vec4i l = lines[i];
    	line (mat_frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
    }
}

void Sobel(void)
{
    Mat mat_frame(frame);
    Mat src_gray, grad_x, grad_y, abs_grad_x, abs_grad_y, grad;
    int delta = 0, scale = 1, ddepth = CV_32F;

    GaussianBlur(mat_frame, mat_frame, Size(3, 3), 0, 0, BORDER_DEFAULT);

    // convert to gray
    cvtColor(mat_frame, src_gray, CV_RGB2GRAY);

    // Gradient X
    Sobel(src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_x, abs_grad_x);

    // Gradient Y
    Sobel(src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_y, abs_grad_y);

    // Total Gradient (approximate)
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
}

void *framerate_tests(void *arg)
{
    cpu_set_t *cores;
    unsigned int max_time = 0, min_time = UINT_MAX, avg_time = 0, deadline_misses = 0, deadline_jitter_min = UINT_MAX,
        deadline_jitter_avg = 0, deadline_jitter_max = 0, runtime_msec = 0, last_res_index = UINT_MAX;
    int remaining_time, retval;
    struct timespec start_time, end_time, delta;
    CvCapture *capture;

    // Allocate one core for this thread
    cores = CPU_ALLOC(1);

    // Bind this thread to core 0.
    if (pthread_setaffinity_np(pthread_self(), sizeof(cores), cores) < 0)
        perror("pthread_setaffinity_np");

    // Initialize the capture
    capture = cvCreateCameraCapture(dev);

    while (1)
    {
        // Wait here for the main thread to signal this thread to start
        if ((retval = sem_wait(&frame_signal)) != 0)
    	    perror("sem_wait");

	if (test_complete)
	    break;

        // Set the capture resolution
	if (last_res_index != res_index)
	{
	    last_res_index = res_index;
            cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, resolutions[res_index][0]);
            cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, resolutions[res_index][1]);
            printf("Setting image size of %dx%d\n", resolutions[res_index][0], resolutions[res_index][1]);
	}

        clock_gettime(CLOCK_REALTIME, &start_time);
        frame = cvQueryFrame(capture);

        if (!frame)
            printf("failed to capture frame!");
        else
        {
            switch(transform_index)
            {
                case CANNY:
                    CannyThreshold(0, 0);
                    break;

                case SOBEL:
                    Sobel();
                    break;

                case HOUGH:
                    HoughTransform();
                    break;

                default:
                    break;
            }

            clock_gettime(CLOCK_REALTIME, &end_time);
            delta_t(&end_time, &start_time, &delta);

    		runtime_msec = delta.tv_sec * SEC_TO_MSEC + delta.tv_nsec / NSEC_TO_MSEC;

            avg_time += runtime_msec;
            if (runtime_msec > max_time)
        		max_time = runtime_msec;
            else if (runtime_msec < min_time)
            	min_time = runtime_msec;

            // Calculate the time remaining for this frame
            remaining_time = ((int) DEADLINE_MS) - ((int) runtime_msec);
            if (remaining_time < 0)
            {
                deadline_misses++;
            }
            else
            {
                // Deadline misses won't be factored into the max, avg and min times.
                if (deadline_jitter_max < remaining_time)
                    deadline_jitter_max = remaining_time;
                else if (deadline_jitter_min > remaining_time)
                    deadline_jitter_min = remaining_time;

                deadline_jitter_avg += remaining_time;
            }

            frame_count++;

            if (frame_count >= N_FRAMES)
            {
            	if (frame_count - deadline_misses == 0)
            	{
            		capture_stats[res_index][transform_index][1] = 0;
            		capture_stats[res_index][transform_index][5] = 0;
            	}
            	else
            	{
            		capture_stats[res_index][transform_index][1] = avg_time / (frame_count - deadline_misses);
            		capture_stats[res_index][transform_index][5] = deadline_jitter_avg / (frame_count - deadline_misses);
            	}

                // Save the stats for this run
                capture_stats[res_index][transform_index][0] = min_time;
                capture_stats[res_index][transform_index][2] = max_time;
                capture_stats[res_index][transform_index][3] = deadline_misses;
                capture_stats[res_index][transform_index][4] = deadline_jitter_min;
                capture_stats[res_index][transform_index][6] = deadline_jitter_max;

                deadline_jitter_min = UINT_MAX; deadline_jitter_avg = 0; deadline_jitter_max = 0;
                max_time = 0; min_time = UINT_MAX; avg_time = 0; deadline_misses = 0;
            }

	    sem_post(&capture_complete);
        }
    }

    // Close the capture
    cvReleaseCapture(&capture);

    // Free the core this thread is using
    CPU_FREE(cores);

    pthread_exit(NULL);
    return NULL;
}

int main( int argc, char** argv )
{
    struct timespec start_time, end_time, delta;
    unsigned int delta_ms;
    int retval;

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
    pthread_attr_init(&frame_attr);
    pthread_attr_setinheritsched(&frame_attr, PTHREAD_INHERIT_SCHED);

    // Set priorities
    main_param.__sched_priority = sched_get_priority_max(SCHED_FIFO);
    frame_param.__sched_priority = main_param.__sched_priority - 1;
    //frame_param.__sched_priority = sched_get_priority_max(SCHED_FIFO);
    //main_param.__sched_priority = frame_param.__sched_priority - 1;

    // Apply scheduling attributes
    sched_setscheduler(getpid(), SCHED_FIFO, &main_param);
    sched_setscheduler(getpid(), SCHED_FIFO, &frame_param);
    pthread_attr_setschedparam(&main_attr, &main_param);
    pthread_attr_setschedparam(&frame_attr, &frame_param);

    print_scheduler();

    sem_init(&frame_signal, 1, 0);
    sem_init(&capture_complete, 1, 0);

    pthread_create(&frame_thread, &frame_attr, framerate_tests, (void *) 0);

    clock_gettime(CLOCK_REALTIME, &start_time);

    while (!test_complete)
    {
        clock_gettime(CLOCK_REALTIME, &end_time);
        delta_t(&end_time, &start_time, &delta);
        delta_ms = delta.tv_nsec / NSEC_TO_MSEC;

        if (delta_ms >= DEADLINE_MS)
        {
            if ((retval = sem_post(&frame_signal)) != 0)
        		perror("sem_post");
            if ((retval = sem_wait(&capture_complete)) != 0)
            	perror("sem_wait");

            if (frame_count >= N_FRAMES)
            {
                frame_count = 0;
                transform_index++;
                if (transform_index >= N_TRANSFORMS)
                {
                    transform_index = 0;
                    res_index++;
                    if (res_index >= N_RESOLUTIONS)
                    {
                        test_complete = true;
                        sem_post(&frame_signal);
                        pthread_join(frame_thread, 0);
                    }
                }

            }

            clock_gettime(CLOCK_REALTIME, &start_time);
        }


    }

    sem_destroy(&frame_signal);
    sem_destroy(&capture_complete);

    printf("Summary of Canny Transform runtimes over 100 frames:\n");
    for (int i = 0; i < N_RESOLUTIONS; i++)
    {
	printf("%dx%d: Min: %fms - Avg: %fms - Max: %fms\n", resolutions[i][0], resolutions[i][1],
	    capture_stats[i][0][0], capture_stats[i][0][1], capture_stats[i][0][2]);
	printf("          Missed deadlines: %d - Jitter: Min: %fms - Avg: %fms - Max: %fms\n",
	   (unsigned int)  capture_stats[i][0][3], capture_stats[i][0][4], capture_stats[i][0][5],
	    capture_stats[i][0][6]);
    }

    printf("\nSummary of Hough Transform runtimes over 100 frames:\n");
    for (int i = 0; i < N_RESOLUTIONS; i++)
    {
   	 printf("%dx%d: Min: %fms - Avg: %fms - Max: %fms\n", resolutions[i][0], resolutions[i][1],
	        capture_stats[i][1][0], capture_stats[i][1][1], capture_stats[i][1][2]);
	 printf("          Missed deadlines: %d - Jitter: Min: %fms - Avg: %fms - Max: %fms\n",
	   (unsigned int)  capture_stats[i][1][3], capture_stats[i][1][4], capture_stats[i][1][5],
	    capture_stats[i][1][6]);
    }

    printf("\nSummary of Sobel Transform runtimes over 100 frames:\n");
    for (int i = 0; i < N_RESOLUTIONS; i++)
    {
	 printf("%dx%d: Min: %fms - Avg: %fms - Max: %fms\n", resolutions[i][0], resolutions[i][1],
	        capture_stats[i][2][0], capture_stats[i][2][1], capture_stats[i][2][2]);
	 printf("          Missed deadlines: %d - Jitter: Min: %fms - Avg: %fms - Max: %fms\n",
	   (unsigned int)  capture_stats[i][2][3], capture_stats[i][2][4], capture_stats[i][2][5],
	    capture_stats[i][2][6]);
    }

    printf("\nSummary of capture runtimes over 100 frames:\n");
	for (int i = 0; i < N_RESOLUTIONS; i++)
	{
	 printf("%dx%d: Min: %fms - Avg: %fms - Max: %fms\n", resolutions[i][0], resolutions[i][1],
			capture_stats[i][3][0], capture_stats[i][3][1], capture_stats[i][3][2]);
	 printf("          Missed deadlines: %d - Jitter: Min: %fms - Avg: %fms - Max: %fms\n",
	   (unsigned int)  capture_stats[i][3][3], capture_stats[i][3][4], capture_stats[i][3][5],
		capture_stats[i][3][6]);
	}

}
