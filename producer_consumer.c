#include "semaphore.h"
#include "userthread.h"

#define	N	100		
#define MAXNUM 1000
int queue[N];
typedef	int	semaphore;
semaphore	mutex = 1;	
semaphore	empty = N;	
semaphore	full = 0;

csem_p cmutex;
csem_p cempty;
csem_p cfull;
int curIdx = 1;

void producer(void* arg)  
{
    while(1)
    {
        down(cempty);		
        down(cmutex);		
        //cs
        if(curIdx <= MAXNUM){
          for(int i=0; i< N; i++)
            if(queue[i]== 0){
              queue[i] = curIdx;
              //printf(2,"put %d in place %d\n", curIdx, i);
              curIdx++; 
              break;
            }
        }
        up(cmutex);		
        up(cfull);	
    }   
    uthread_exit();
}

void consumer(void* arg)
{
    int timeToSleep;
    while(1)	
    {
        down(cfull);		
        down(cmutex);	                  
        for(int i=0; i< N; i++)
          if(queue[i]!= 0){
            timeToSleep =  queue[i];
            queue[i] = 0;
            break;
          }
        up(cmutex);		   
        up(cempty);		
        uthread_sleep(timeToSleep);
        if(timeToSleep != 0)
          printf(2,"thread %d slept for %d ticks\n", uthread_self(), timeToSleep);
        if(timeToSleep == MAXNUM){
          exit();
        }
        timeToSleep = 0;
    }
}

int 
main(int argc, char *argv[]) 
{
  int* nothing = 0;
  uthread_init();

  cmutex = csem_alloc(mutex);
  cempty = csem_alloc(empty);
  cfull = csem_alloc(full);

    
  uthread_create(producer, nothing);
  uthread_create(consumer, nothing);
  uthread_create(consumer, nothing);
  uthread_create(consumer, nothing);
  
  
  uthread_exit();

  
  return 1;
}


/*void mythread(void* arg)
{
  int i;
  printf(1, "thread %d: running\n", uthread_self());
  uthread_sleep(10);
  bsem_down(0);

  for (i = 0; i < 20; i++) {
      if(i==5){
       uthread_sleep(250); 
       printf(2,"thread %d woke up\n",uthread_self());
      }
    printf(1, "thread %d says hello\n", uthread_self());
  }
    bsem_up(0);

  printf(1, "thread %d: exit\n", uthread_self());
  uthread_exit();
}


void mythread1(void* arg)
{
  int i;
  bsem_down(0);

  printf(1, "thread %d: running\n", uthread_self());
  for (i = 0; i < 20; i++) {
    printf(1, "thread %d says bye bye\n", uthread_self());
  }
  bsem_up(0);
  printf(1, "thread %d: exit\n", uthread_self());
  uthread_exit();
    
}


int 
main(int argc, char *argv[]) 
{
  int* nothing = 0;
  uthread_init();
  bsem_alloc();
    
  uthread_create(mythread, nothing);
  uthread_create(mythread1, nothing);



  uthread_join(1);
  uthread_join(2);
  
  int i;
  printf(1, "thread %d: running\n", uthread_self());
  for (i = 0; i < 20; i++) {
    printf(1, "thread %d says im the master of the uni\n", uthread_self());
  }
  printf(1, "thread %d: exit\n", uthread_self());
  
  uthread_exit();

  
  return 1;
}
*/