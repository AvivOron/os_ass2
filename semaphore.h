#define MAX_BSEM 	128
#define FREE_SEM        0x0
#define ALLOCATED     0x1

typedef struct bsem bsem, *bsem_p;

struct bsem {
  int  locked;     
  int descriptor;           
  int state;
};


int bsem_alloc();
void bsem_free(int bidx);
void bsem_down(int bidx);
void bsem_up(int bidx);