#include "semaphore.h"
#include "userthread.h"
/*
#define	N	100		
ypedef	int	semaphore;
semaphore	mutex = 1;	
semaphore	empty = N;	
semaphore	full = 0; 

void producer(void)  
{
    int	item;
    while(TRUE)  pro
    {
        produce_item(&item);	
        down(&empty);		
        down(&mutex);		
        enter_item(item);	
        up(&mutex);		
        up(&full);		
    }   

}

void consumer(void)
{
    int item;
    while(TRUE)	
    {
        down(&full);		
        down(&mutex);	                  
        remove_item(&item);	
        up(&mutex);		   
        up(&empty);		
        consume_item(item);		
    }
}
*/

void mythread(void* arg)
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