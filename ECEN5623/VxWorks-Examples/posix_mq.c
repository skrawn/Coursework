/****************************************************************************/
/* Function: Basic POSIX message queue demo from VxWorks Prog. Guide p. 78  */
/*                                                                          */
/* Sean Donohue - 06/27/2016                                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
                                                                    
#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define SNDRCV_MQ "/send_receive_mq"
#define MAX_MSG_SIZE 128
#define ERROR (-1)

struct mq_attr mq_attr;

pthread_t sender_thread, receiver_thread;
pthread_attr_t sender_attr, receiver_attr;  
struct sched_param sender_param, receiver_param;

void *receiver(void *arg)
{
  mqd_t mymq;
  char buffer[MAX_MSG_SIZE];
  int prio;
  int nbytes;

  /* note that VxWorks does not deal with permissions? */
  mymq = mq_open(SNDRCV_MQ, O_CREAT|O_RDWR, 0, &mq_attr);

  if(mymq == (mqd_t)ERROR)
    perror("mq_open");

  /* read oldest, highest priority msg from the message queue */
  if((nbytes = mq_receive(mymq, buffer, MAX_MSG_SIZE, &prio)) == ERROR)
  {
    perror("mq_receive");
  }
  else
  {
    buffer[nbytes] = '\0';
    printf("receive: msg %s received with priority = %d, length = %d\n",
           buffer, prio, nbytes);
  }
    
}

static char canned_msg[] = "this is a test, and only a test, in the event of a real emergency, you would be instructed ...";

void *sender(void *arg)
{
  mqd_t mymq;
  int prio;
  int nbytes;

  /* note that VxWorks does not deal with permissions? */
  mymq = mq_open(SNDRCV_MQ, O_RDWR, 0, &mq_attr);

  if(mymq == (mqd_t)ERROR)
    perror("mq_open");

  /* send message with priority=30 */
  if((nbytes = mq_send(mymq, canned_msg, sizeof(canned_msg), 30)) == ERROR)
  {
    perror("mq_send");
  }
  else
  {
    printf("send: message successfully sent\n");
  }
  
}


void mq_demo(void)
{

  /* setup common message q attributes */
  mq_attr.mq_maxmsg = 100;
  mq_attr.mq_msgsize = MAX_MSG_SIZE;

  mq_attr.mq_flags = 0;

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


  /* receiver runs at a higher priority than the sender */
  if(pthread_create(&receiver_thread, &receiver_attr, receiver, (void *) 0) == ERROR) {
    printf("Receiver task spawn failed\n");
  }
  else
    printf("Receiver task spawned\n");

  if(pthread_create(&sender_thread, &sender_attr, sender, (void *) 0) == ERROR) {
    printf("Sender task spawn failed\n");
  }
  else
    printf("Sender task spawned\n");

   pthread_join(sender_thread, 0);
   pthread_join(receiver_thread, 0);
}

void main(void)
{
	mq_demo();
}
