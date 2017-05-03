#include "types.h"
#include "stat.h"
#include "user.h"
#include "x86.h"

#define	N	100		/* Buffer size */
typedef	int	semaphore;
semaphore	mutex = 1;	/* access control to critical section */
semaphore	empty = N;	/* counts empty buffer slots */
semaphore	full = 0; /* counts full slots */

void producer(void)  
{
    int	item;
    while(TRUE) 
    {
        produce_item(&item);	/* generate something... */
        down(&empty);		/* decrement count of empty */
        down(&mutex);		/* enter critical section */
        enter_item(item);	/* insert into buffer */
        up(&mutex);		/* leave critical section */
        up(&full);		/* increment count of full slots */
    }   

}

void consumer(void)
{
    int item;
    while(TRUE)	
    {
        down(&full);		/* decrement count of full */   
        down(&mutex);	                /* enter critical section */	   
        remove_item(&item);	/* take item from buffer) */
        up(&mutex);		/* leave critical section */	   
        up(&empty);		/* update count of empty */   
        consume_item(item);	/* do something... */		
    }
}
