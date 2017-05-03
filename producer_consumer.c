#include "types.h"
#include "stat.h"
#include "user.h"
#include "x86.h"
/*
#define	N	100		
ypedef	int	semaphore;
semaphore	mutex = 1;	
semaphore	empty = N;	
semaphore	full = 0; 

void producer(void)  
{
    int	item;
    while(TRUE) 
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