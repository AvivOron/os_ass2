#define MAX_BSEM 	128
#define FREE_SEM        0x0
#define ALLOCATED     0x1

typedef struct bsem bsem, *bsem_p;
typedef struct counting_semaphore counting_semaphore, *csem_p;


struct bsem {
  int  locked;     
  int descriptor;           
  int state;
};

struct counting_semaphore{
	int value, initialValue;
	bsem_p bsem1;
	bsem_p bsem2;
};


int bsem_alloc();
void bsem_free(int bidx);
void bsem_down(int bidx);
void bsem_up(int bidx);

void down(struct counting_semaphore *sem);
void up(struct counting_semaphore *sem);
struct counting_semaphore * csem_alloc(int val);
void csem_free(struct counting_semaphore *sem);