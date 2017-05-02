#include "types.h"
#include "stat.h"
#include "user.h"
#include "x86.h"

/* Possible states of a thread; */
#define FREE        0x0
#define RUNNING     0x1
#define RUNNABLE    0x2

#define STACK_SIZE  4096
#define MAX_UTHREADS 64
#define UTHREAD_QUANTA 5

typedef struct thread thread_t, *thread_p;
typedef struct trapframe *tf; 
uint localEsp;

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

  thread_p t;
  if(current_thread->state == RUNNING)
  {
    current_thread->state = RUNNABLE;
  }

  /* Find another runnable thread. */
  alarm(0);
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
  alarm(UTHREAD_QUANTA);

  if (t >= all_thread + MAX_UTHREADS && current_thread->state == RUNNABLE) {
    /* The current thread is the only runnable thread; run it. */
    next_thread = current_thread;
    //printf(2, "keep running thread %d\n", current_thread->id);
  }

  if (next_thread == 0) {
    printf(2, "thread_schedule: no runnable threads; deadlock\n");
    exit();
  }
  
  
  if (current_thread != next_thread ) {         /* switch threads  */
    //if first execution of thread - only override the tf fields
      
    asm("movl %%esp, %0\n\t"
    : "=r" (localEsp)
    :
    );
        
    if(!next_thread->executed && next_thread->id !=0){
        next_thread->state = RUNNING;

        uint tfaddrs = localEsp + 20;
        printf(2,"");   

      
        memmove((void*)(&current_thread->oldtf),(void*)(tfaddrs), sizeof(struct trapframe));
        memmove((void*)(&next_thread->oldtf),(void*)(tfaddrs), sizeof(struct trapframe));
        
        next_thread->oldtf.ebp = next_thread->ebp;
        next_thread->oldtf.eip = next_thread->eip;
        next_thread->oldtf.esp = next_thread->esp;

        memmove((void*)(tfaddrs),(void*)(&next_thread->oldtf), sizeof(struct trapframe));

        next_thread->executed = 1;
    }
    else{
      //not first time running
      uint tfaddrs = localEsp + 20;
      printf(2,"");   
      
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
  printf(2, "exitting\n");
  current_thread->state = FREE;
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

   alarm(UTHREAD_QUANTA);
    //printf(2, "ebp create: %d, id: %d\n",t->ebp, t->id);
    return index;
  }
  alarm(UTHREAD_QUANTA);
  return -1;
}




int uthread_join(int tid)
{
  thread_p t;
  for (t = all_thread; t < all_thread + MAX_UTHREADS; t++) {
    if (t->id == tid)
    {
        while(!t->state == FREE) { }
        break;
    }
  }
  
  return 0;
}

int uthread_sleep(int ticks)
{
    int i = ticks;
    while(--i) { }
    return 0;
}


void mythread(void* arg)
{
  int i;
  printf(1, "thread %d: running\n", uthread_self());
  for (i = 0; i < 100; i++) {
    printf(1, "thread %d says hello\n", uthread_self());
  }
  printf(1, "thread %d: exit\n", uthread_self());
  current_thread->state = FREE;
}

void mythread1(void* arg)
{
  int i;
  printf(1, "thread %d: running\n", uthread_self());
  for (i = 0; i < 100; i++) {
    printf(1, "thread %d says bye bye\n", uthread_self());
  }
  printf(1, "thread %d: exit\n", uthread_self());
  current_thread->state = FREE;
}


int 
main(int argc, char *argv[]) 
{
  int* nothing = 0;
  uthread_init();
  uthread_create(mythread, nothing);
  uthread_create(mythread1, nothing);
  sleep(5);

  uthread_create(mythread, nothing);
  uthread_create(mythread1, nothing);

    sleep(5);

  uthread_create(mythread, nothing);

  sleep(5);
  /*uthread_create(mythread, nothing);
  */
  exit();
  return 1;
}