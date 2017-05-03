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
  int bidx;
};

 thread_t all_thread[MAX_UTHREADS];
 thread_p  current_thread;

int uthread_sleep(int ticks);
int uthread_join(int tid);
int uthread_create(void (*start_func)(void *), void*arg);
void uthread_exit();
int uthread_init();
int uthread_self();
int uthread_state();