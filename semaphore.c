#include "userthread.h"
#include "semaphore.h"

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
                        printf(2, "thread %d passed sleep(-1);", uthread_self());
		}
		alarm(0);
		if(b->state == ALLOCATED){
			printf(2,"thead %d (%d) locked %d which its locked status is %d\n", uthread_self(),uthread_state(), bidx, b->locked);
			b->locked = 1;
		}
	}
	sigsend(getpid(), 14);
}

void bsem_up(int bidx){
    printf(2,"UP\n");
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