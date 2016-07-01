/****************************************************************************/
/*                                                                          */                                               
/* Sean Donohue - 06/16/2016                                                */
/*                                                                          */
/****************************************************************************/
                                                                    
#include <mqueue.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SNDRCV_MQ "/send_receive_mq"
#define ERROR		(-1)

struct mq_attr mq_attr;
static mqd_t mymq;

pthread_t sender_thread, receiver_thread;
pthread_attr_t sender_attr, receiver_attr;  
struct sched_param sender_param, receiver_param;

/* receives pointer to heap, reads it, and deallocate heap memory */

void *receiver(void *arg)
{
  char buffer[sizeof(void *)+sizeof(int)];
  void *buffptr; 
  int prio;
  int nbytes;
  int count = 0;
  int id;
 
  while(1) 
  {

    /* read oldest, highest priority msg from the message queue */

    printf("Reading %ld bytes\n", sizeof(void *));
  
    if((nbytes = mq_receive(mymq, buffer, (size_t)(sizeof(void *)+sizeof(int)), &prio)) == ERROR)
    {
      perror("mq_receive");
    }
    else
    {
      memcpy(&buffptr, buffer, sizeof(void *));
      memcpy((void *)&id, &(buffer[sizeof(void *)]), sizeof(int));
      printf("receive: ptr msg 0x%X received with priority = %d, length = %d, id = %d\n", buffptr, prio, nbytes, id);

      printf("contents of ptr = \n%s\n", (char *)buffptr);

      free(buffptr);

      printf("heap space memory freed\n");

    }
    
  }

}


static char imagebuff[4096];

void *sender(void *arg)
{
  char buffer[sizeof(void *)+sizeof(int)];
  void *buffptr;
  int prio;
  int nbytes;
  int id = 999;

  while(1) {

    /* send malloc'd message with priority=30 */

    buffptr = (void *)malloc(sizeof(imagebuff));
    strcpy(buffptr, imagebuff);
    printf("Message to send = %s\n", (char *)buffptr);

    printf("Sending %ld bytes\n", sizeof(buffptr));

    memcpy(buffer, &buffptr, sizeof(void *));
    memcpy(&(buffer[sizeof(void *)]), (void *)&id, sizeof(int));

    //if((nbytes = mq_send(mymq, buffer, (size_t)(sizeof(void *)+sizeof(int)), 30)) == ERROR)
    if((nbytes = mq_send(mymq, buffer, (size_t)(sizeof(void *)), 30)) == ERROR)
    {
      perror("mq_send");
    }
    else
    {
      printf("send: message ptr 0x%X successfully sent\n", buffptr);
    }

    usleep(3000000);
  }
  
}


static int sid, rid;

void heap_mq_posix(void)
{
  int i, j;
  char pixel = 'A';

  pthread_attr_t main_attr;
  struct sched_param main_param;

  pthread_attr_init(&main_attr);
  pthread_attr_setinheritsched(&main_attr, PTHREAD_EXPLICIT_SCHED);
  pthread_attr_setschedpolicy(&main_attr, SCHED_FIFO);

  pthread_attr_init(&sender_attr);
  pthread_attr_setinheritsched(&sender_attr, PTHREAD_INHERIT_SCHED);

  pthread_attr_init(&receiver_attr);
  pthread_attr_setinheritsched(&receiver_attr, PTHREAD_INHERIT_SCHED);

  main_param.__sched_priority = sched_get_priority_max(SCHED_FIFO);
  receiver_param.__sched_priority = main_param.__sched_priority - 1;
  sender_param.__sched_priority = receiver_param.__sched_priority - 1;

  sched_setscheduler(getpid(), SCHED_FIFO, &main_param);
  sched_setscheduler(getpid(), SCHED_FIFO, &sender_param);
  sched_setscheduler(getpid(), SCHED_FIFO, &receiver_param);
  pthread_attr_setschedparam(&main_attr, &main_param);
  pthread_attr_setschedparam(&sender_attr, &sender_param);
  pthread_attr_setschedparam(&receiver_attr, &receiver_param);

  for(i=0;i<4096;i+=64) {
    pixel = 'A';
    for(j=i;j<i+64;j++) {
      imagebuff[j] = (char)pixel++;
    }
    imagebuff[j-1] = '\n';
  }
  imagebuff[4095] = '\0';
  imagebuff[63] = '\0';

  printf("buffer =\n%s\n", imagebuff);

  /* setup common message q attributes */
  mq_attr.mq_maxmsg = 100;
  mq_attr.mq_msgsize = sizeof(void *);
  printf("msgsize: %ld\n", mq_attr.mq_msgsize);

//  mq_attr.mq_flags = 0;
  mq_attr.mq_flags = O_CREAT|O_RDWR;

  /* note that VxWorks does not deal with permissions? */
//  mymq = mq_open(SNDRCV_MQ, O_CREAT|O_RDWR, 0, &mq_attr);
  mymq = mq_open(SNDRCV_MQ, O_CREAT|O_RDWR, S_IRWXU, &mq_attr);

  if(mymq == (mqd_t)ERROR)
    perror("mq_open");

  /* receiver runs at a higher priority than the sender */
  if((rid=pthread_create(&receiver_thread, &receiver_attr, receiver, (void *) 0)) == ERROR) {
    printf("Receiver task spawn failed\n");
  }
  else
    printf("Receiver task spawned\n");

  if((sid=pthread_create(&sender_thread, &sender_attr, sender, (void *) 0)) == ERROR) {
    printf("Sender task spawn failed\n");
  }
  else
    printf("Sender task spawned\n");

   pthread_join(sender_thread, 0);
   pthread_join(receiver_thread, 0);

}

void shutdown(void)
{
  mq_close(mymq);
  pthread_kill(sid);
  pthread_kill(rid);

}

void main(void)
{
	heap_mq_posix();
}


