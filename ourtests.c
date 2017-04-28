#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

sighandler_t tester(int i){
  printf(1,"I am robot %d\n", i);
  return 0;
}

int
main(int argc, char *argv[])
{
  int pid = fork();
  //signal(1,tester);
  if(pid){ //father
    wait();
    printf(1,"my pid is %d\n", pid);
  } 
  else{//son
    sigsend(4,1);
    sigsend(4,2);
    printf(1,"sent signal to %d\n", pid);
  }
  exit();
}
