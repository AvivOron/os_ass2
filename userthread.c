#include "types.h"
#include "stat.h"
#include "user.h"
#include "x86.h"

/* Possible states of a thread; */
#define FREE        0x0
#define RUNNING     0x1
#define RUNNABLE    0x2
#define WAITING    0x3
#define SLEEPING    0x4

#define STACK_SIZE  4096
#define MAX_UTHREADS 64
#define UTHREAD_QUANTA 5

typedef struct thread thread_t, *thread_p;
typedef struct trapframe *tf; 
uint localEbp;

struct thread {
  int  sp;                /* curent stack pointer */
  char stack[STACK_SIZE];       /* the thread's stack */
  struct trapframe oldtf;        // Trap frame for current syscall  
  uint ebp;
  uint esp;
  uint eip;
  int  state;             /* running, runnable, waiting */
  int id;
  int executed;
  int waitingFor;
  int wentToSleepAt;
  int ticksToSleep;
};

static thread_t all_thread[MAX_UTHREADS];
static thread_p  current_thread;
static thread_p  next_thread;
 

int uthread_self()
{
    return current_thread->id;
}


void uthread_schedule()
{
  int sleepingThreads = 0;
  int init = 1;
  thread_p t;
  alarm(0);
  
  if(current_thread->state == RUNNING)
    current_thread->state = RUNNABLE;

  while(init || sleepingThreads)
  {
    init = 0;  
    sleepingThreads = 0;
    
    //wake up sleeping threads that need to be up
    for(t=all_thread; t<all_thread+MAX_UTHREADS; t++){
        if(t->state == SLEEPING)
        {
            if(t->wentToSleepAt+t->ticksToSleep > uptime())
            {
                sleepingThreads = 1;   
            }
            else
            {
                t->state = RUNNABLE;
            }
        }
    }
    /* Find another runnable thread. */
    for (t = current_thread; t < all_thread + MAX_UTHREADS; t++) {
        if (t->state == RUNNABLE && t != current_thread) {
        next_thread = t;
        break;
        }
    }
    if(next_thread == 0){
        for (t = all_thread; t < current_thread; t++) {
            if (t->state == RUNNABLE && t != current_thread) {
            next_thread = t;
            break;
            }
        } 
    }
    //alarm(UTHREAD_QUANTA);

    if (next_thread == 0 && current_thread->state == RUNNABLE) {
        /* The current thread is the only runnable thread; run it. */
        next_thread = current_thread;
        //printf(2, "keep running thread %d\n", current_thread->id);
    }
  
    if(next_thread != 0)
    {
     printf(2, "thread %d chosen\n", next_thread->id);
     break;   
    }
  
  }

  if (next_thread == 0) {
    printf(2, "thread_schedule: no runnable threads; deadlock\n");
    exit();
  }
  
  
  if (current_thread != next_thread ) {         /* switch threads  */
    //if first execution of thread - only override the tf fields
      
    asm("movl %%ebp, %0\n\t"
    : "=r" (localEbp)
    :
    );
        
    if(!next_thread->executed && next_thread->id !=0){
        next_thread->state = RUNNING;
        uint tfaddrs = localEbp + 12;
  

      
        memmove((void*)(&current_thread->oldtf),(void*)(tfaddrs), sizeof(struct trapframe));
        memmove((void*)(&next_thread->oldtf),(void*)(tfaddrs), sizeof(struct trapframe));
        
        next_thread->oldtf.ebp = next_thread->ebp;
        next_thread->oldtf.eip = next_thread->eip;
        next_thread->oldtf.esp = next_thread->esp;

        memmove((void*)(tfaddrs),(void*)(&next_thread->oldtf), sizeof(struct trapframe));

        next_thread->executed = 1;
    }
    else{
      //not first time running thread
      uint tfaddrs = localEbp + 12;
      
      memmove((void*)(&current_thread->oldtf),(void*)(tfaddrs), sizeof(struct trapframe));
      memmove((void*)(tfaddrs),(void*)(&next_thread->oldtf), sizeof(struct trapframe));
      
    }
    current_thread = next_thread;   
    next_thread = 0;
  }
  else
    next_thread = 0;

  alarm(UTHREAD_QUANTA);

}

int uthread_init()
{
  current_thread = &all_thread[0];
  current_thread->state = RUNNING;
  signal(14, uthread_schedule);
  alarm(UTHREAD_QUANTA);

  return 1;
}

void 
uthread_exit()
{
  alarm(0);
  printf(2, "thread %d exitting\n", uthread_self());
  int flag;
  thread_p t;

  current_thread->state = FREE;
  current_thread->sp = 0;
  //current_thread->stack=0;       /* the thread's stack */
  //current_thread->oldtf=0;        // Trap frame for current syscall  
  current_thread->ebp = 0;
  current_thread->esp =0;
  current_thread->eip=0;
  current_thread->state=0;             /* running, runnable, waiting */
  current_thread->executed=0;
  current_thread->wentToSleepAt =0;
  current_thread->ticksToSleep = 0;
  for (t = all_thread; t < all_thread + MAX_UTHREADS; t++) {
    if (t->state == RUNNABLE && t->id != 0) 
      flag = 1;
    //wake up threads waiting for me
    if (t->state == WAITING && t->waitingFor == current_thread->id)
    {
      printf(2, "thread %d is no longer waiting for thread %d\n", t->id, current_thread->id);    
      t->state = RUNNABLE;
    }
    //wake sleeping threads up
    if(t->state == SLEEPING && t->wentToSleepAt+t->ticksToSleep <= uptime())
      t->state = RUNNABLE;
  }
  current_thread->id=0;

  

  if(!flag)
  {
    printf(2, "im the last one!\n");
    exit();  
  }
  else{
    int pid = getpid();
    sigsend(pid,14);
  }


}


int 
uthread_create(void (*start_func)(void *), void*arg)
{
  thread_p t;
  int index = 0;
  alarm(0);
  for (t = all_thread; t < all_thread + MAX_UTHREADS; t++) {
    if (t->state == FREE) break;
    index++;
  }
  if(index > 0 && index < MAX_UTHREADS)
  {
    t->sp = (int) (t->stack + STACK_SIZE);   // set sp to the top of the stack
    t->sp -= 4;                
     * (int *) (t->sp) = (int)arg; //saving the func arg          
    t->sp -= 4;                
     * (int *) (t->sp)= (int)uthread_exit; //saving the return address which is exit call
    //t->sp -= 4;     
    t->ebp = t->sp; //backing up the eb/sp to top of the stack
    t->esp = t->sp;
    t->eip = (int)start_func; // set eip to the given function of the thread
    t->state = RUNNABLE;
    t->id = index;
    t->executed = 0;
    t->waitingFor = -1;

   alarm(UTHREAD_QUANTA);
    //printf(2, "ebp create: %d, id: %d\n",t->ebp, t->id);
    return index;
  }
  alarm(UTHREAD_QUANTA);
  return -1;
}

int uthread_join(int tid)
{
  alarm(0);
  thread_p t;
  if(tid >= 0 && tid < MAX_UTHREADS && all_thread[tid].state != FREE){
    printf(2,"thread %d is waiting for thread %d\n",uthread_self(),tid);
    t->waitingFor = tid;
    t->state = WAITING;
  }
  else 
    return -1;
  
  int pid = getpid();
  sigsend(pid,14);
  
  return 0; 
}

int uthread_sleep(int ticks)
{
    alarm(0);
    if(ticks < 0){
      return -1;
    }
    printf(2,"thread %d is falling asleep\n",uthread_self());
    current_thread->ticksToSleep = ticks;
    current_thread->wentToSleepAt = uptime();
    current_thread->state = SLEEPING;
    
    int pid = getpid();
    sigsend(pid,14);

    return 0;
}


void mythread(void* arg)
{
  int i;
  printf(1, "thread %d: running\n", uthread_self());
  for (i = 0; i < 20; i++) {
      if(i==10){
       uthread_sleep(500); 
       printf(2,"thread %d woke up\n",uthread_self());
      }
    printf(1, "thread %d says hello\n", uthread_self());
  }
  printf(1, "thread %d: exit\n", uthread_self());
  uthread_exit();
}


void mythread1(void* arg)
{
  int i;
  printf(1, "thread %d: running\n", uthread_self());
  for (i = 0; i < 20; i++) {
    printf(1, "thread %d says bye bye\n", uthread_self());
  }
  printf(1, "thread %d: exit\n", uthread_self());
  uthread_exit();
    
}


int 
main(int argc, char *argv[]) 
{
  int* nothing = 0;
  uthread_init();
  //uthread_create(mythread_sleep, nothing);
  uthread_create(mythread, nothing);
  uthread_create(mythread1, nothing);
  //uthread_create(mythread1, nothing);
  //uthread_create(mythread1, nothing);

  uthread_join(1);
  //uthread_join(2);
  //uthread_join(3);
  
  uthread_exit();



 /* uthread_join(2);
  uthread_create(mythread, nothing);
  //uthread_create(mythread1, nothing);

  uthread_sleep(5);
  uthread_join(3);

  uthread_sleep(5);
  uthread_sleep(5);
  uthread_sleep(5);
*/
  //uthread_create(mythread, nothing);

  /*uthread_create(mythread, nothing);
  */
  
  return 1;
}