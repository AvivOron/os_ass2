#include "userthread.h"

#define MAX_BSEM 	128
#define FREE_SEM        0x0
#define ALLOCATED     0x1

typedef struct bsem bsem, *bsem_p;

struct bsem {
  int  locked;     
  int descriptor;           
  int state;
};

static bsem all_bsem[MAX_BSEM];

int bsem_alloc(){
	bsem_p b;
  	int index = 0;
  	for (b = all_bsem; b < all_bsem + MAX_BSEM; b++) {
    	if (b->state == FREE_SEM) break;
    	index++;
  	}

  	if(index < MAX_BSEM){	
	b->descriptor = index;
	b->state = ALLOCATED;
	b->locked = 0;
	return index;
  	}
  	return -1;
} 

void bsem_free(int bidx){
	if(bidx >=0 && bidx < MAX_BSEM){
		bsem_p b = &all_bsem[bidx];
		if(b->state == ALLOCATED && !b->locked){
			b->state = FREE_SEM;
			b->locked = 0;
			b->descriptor = -1;
		}
	}
}

void bsem_down(int bidx){
	alarm(0);
	if(bidx >=0 && bidx < MAX_BSEM){
		bsem_p b = &all_bsem[bidx];
		if(b->state == ALLOCATED && b->locked){
			printf(2,"thead %d couldnt lock %d\n", uthread_self(), bidx);
			current_thread->bidx = bidx;
			uthread_sleep(-1);
		}
		alarm(0);
		if(b->state == ALLOCATED){
			printf(2,"thead %d locked %d\n", uthread_self(), bidx);
			b->locked = 1;
		}
	}
	sigsend(getpid(), 14);
}

void bsem_up(int bidx){
	alarm(0);
	if(bidx >=0 && bidx < MAX_BSEM){
		bsem_p b = &all_bsem[bidx];
		if(b->state == ALLOCATED && b->locked){
			thread_p t;
			b->locked = 0;
			for(t = all_thread; t < all_thread+MAX_UTHREADS; t++)
			  if(t->state == SLEEPING && t->bidx == bidx){
			  	printf(2,"thead %d freed from %d\n", t->id, bidx);
			  	t->bidx = -1;
			  	t->state = RUNNABLE;
			  	break;
			  }
	
		}
	}
	sigsend(getpid(), 14);
}


/*
void mythread(void* arg)
{
  int i;
  printf(1, "thread %d: running\n", uthread_self());

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

  printf(1, "thread %d: exit\n", uthread_self());
  uthread_exit();
    
}
*/

int 
main(int argc, char *argv[]) 
{
  //int* nothing = 0;
  uthread_init();
  /*bsem_alloc();
    
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
  
  uthread_exit();*/

  
  return 1;
}