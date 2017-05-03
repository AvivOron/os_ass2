#include "types.h"
#include "stat.h"
#include "user.h"
#include "x86.h"

type semaphore = record
value, flag: integer;
L: list of process;
end;

void down(S)
{
    S.value--;
    if (S.value < 0) 
    {
        add this process to S.L;
        S.flag=0
        sleep;
    }
    else
    {
        S.flag=0 
    }
}


void up(S)
{
    S.value++;
    if (S.value <= 0) 
    {
        remove a process P from S.L;
        wakeup(P);
    }
    S.flag=0
}