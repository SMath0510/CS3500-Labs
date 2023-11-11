# Lab 2
## Adding a user call

1. Add a [CALL].c file in user
2. Add this in the Makefile

---

# Lab 3

## Adding a system call

1. Add the syscall definition in `syscall.c` (two places)
2. `#define` the syscall in `syscall.h`
3. Add your code for the syscall in `sysfile.c` or `sysproc.call`
4. Add the definition in `defs.h`
5. Add the system call in `user.h`
6. Add the system call in `usys.pl`

---

# Lab 4

## Add code for page walk refering to freewalk
``` cpp
void
printwalk(pagetable_t pagetable, int depth)
{
  // there are 2^9 = 512 PTEs in a page table.
  for(int i = 0; i < 512; i++){
    pte_t pte = pagetable[i];
    if((pte & PTE_V)){
      for(int i = 0; i < depth; i++){
        printf(".. ");
      }
      printf("..%d: pte %p pa %p\n", i, pte, PTE2PA(pte));
    
      if((pte & (PTE_R|PTE_W|PTE_X)) == 0){
        // this PTE points to a lower-level page table.
        uint64 child = PTE2PA(pte);
        printwalk((pagetable_t)child, depth+1);
      }
    }
  }
}

void 
vmprint(pagetable_t pagetable){
  printf("page table %p\n", pagetable);
  printwalk(pagetable, 0);
}
```

In `exec.c` add in the command 

```cpp
if (process -> pid == 1) {
    vmprint(process -> pagetable) ;
}
```
---
# Lab 5

Code for bttest.c in user

``` cpp
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  sleep(2);
  exit(0);
}
```

Define backtrace in defs.h in printf.c

```cpp
void            backtrace(void);
```

Call backtrace in panic() in printf.c

Define r_fp() in riscv.h

``` cpp
static inline uint64 
r_fp()
{
  uint64 x;
  asm volatile("mv %0, s0" : "=r" (x) ); 
  return x;
}
```

Write the code for backtrace in printf.c
``` cpp
void 
backtrace(void)
{
  printf("backtrace: \n");
  
  uint64 * fp = (uint64 *) r_fp();
  uint64 bottom = PGROUNDUP(*fp);

  while(((uint64) fp) < bottom)
  {
    uint64 * ra = (uint64 *) (fp - 1);
    fp = (uint64 *) (*(fp - 2));
    printf("%p\n", ra);
  }
}
```

---
# Lab 6

uthreads.c code
``` cpp
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/* Possible states of a thread: */
#define FREE        0x0
#define RUNNING     0x1
#define RUNNABLE    0x2

#define STACK_SIZE  8192
#define MAX_THREAD  4
#define MATRIX_SIZE 3

int A[MATRIX_SIZE][MATRIX_SIZE] = {{7,2,5},{3,7,8},{1,1,7}};
int B[MATRIX_SIZE][MATRIX_SIZE] = {{3,0,0},{0,3,0},{0,0,3}};
int C[MATRIX_SIZE][MATRIX_SIZE] = {{0,0,0},{0,0,0},{0,0,0}};

typedef struct _my_regs{
uint64 ra;
uint64 sp;

// additional regs
uint64 s0;
uint64 s1;
uint64 s2;
uint64 s3;
uint64 s4;
uint64 s5;
uint64 s6;
uint64 s7;
uint64 s8;
uint64 s9;
uint64 s10;
uint64 s11;
} my_regs_t;

struct thread {
  char       stack[STACK_SIZE]; /* the thread's stack */
  int        state;             /* FREE, RUNNING, RUNNABLE */
  my_regs_t my_regs;     // the registers of the
};
struct thread all_thread[MAX_THREAD];
struct thread *current_thread;
extern void thread_switch(uint64, uint64);
              


void print_res(){
  for(int i = 0; i < MATRIX_SIZE; i++){
    for(int j = 0; j < MATRIX_SIZE; j++){
      printf("%d ", C[i][j]);
    }
    printf("\n");
  }
}


void 
thread_init(void)
{
  // main() is thread 0, which will make the first invocation to
  // thread_schedule().  it needs a stack so that the first thread_switch() can
  // save thread 0's state.  thread_schedule() won't run the main thread ever
  // again, because its state is set to RUNNING, and thread_schedule() selects
  // a RUNNABLE thread.
  current_thread = &all_thread[0];
  current_thread->state = RUNNING;
}

void 
thread_schedule(void)
{
  struct thread *t, *next_thread;

  /* Find another runnable thread. */
  next_thread = 0;
  t = current_thread + 1;
  for(int i = 0; i < MAX_THREAD; i++){
    if(t >= all_thread + MAX_THREAD)
      t = all_thread;
    if(t->state == RUNNABLE) {
      next_thread = t;
      break;
    }
    t = t + 1;
  }

  if (next_thread == 0) {
    print_res();
    printf("thread_schedule: no runnable threads\n");
    exit(-1);
  }

  if (current_thread != next_thread) {         /* switch threads?  */
    next_thread->state = RUNNING;
    t = current_thread;
    current_thread = next_thread;
    /* YOUR CODE HERE
     * Invoke thread_switch to switch from t to next_thread:
     * thread_switch(??, ??);
     */
    thread_switch((uint64)&(t->my_regs), (uint64)&(next_thread->my_regs));
  } else
    next_thread = 0;
}

void 
thread_create(void (*func)())
{
  struct thread *t;

  for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
    if (t->state == FREE) break;
  }
  t->state = RUNNABLE;
  // YOUR CODE HERE
  memset(&t->my_regs, 0, sizeof(t->my_regs)); // initialize to 0
  t->my_regs.ra = (uint64) func;
  t->my_regs.sp = (uint64)(t->stack + STACK_SIZE);
}

void 
thread_yield(void)
{
  current_thread->state = RUNNABLE;
  thread_schedule();
}

volatile int a_started, b_started, c_started;
volatile int a_n, b_n, c_n;

void 
multiply(int row, char thread_name)
{
  for(int col = 0; col < MATRIX_SIZE; col++){
    int sum = 0;
    for(int i = 0; i < MATRIX_SIZE; i++){
      sum += A[row][i] * B[i][col];
    }
    C[row][col] = sum;
    printf("thread %c: %d\n", thread_name, sum);
  }
}

void 
thread_a(void)
{
  printf("thread_a started\n");
  a_started = 1;

  while(b_started == 0 || c_started == 0)
    thread_yield();

  // multiply(0,'a');
  // thread_yield();
  C[0][0] = A[0][0] * B[0][0] + A[0][1] * B[1][0] + A[0][2] * B[2][0];
  printf("thread a : %d\n", C[0][0]); 
  thread_yield();
  C[0][1] = A[0][0] * B[0][1] + A[0][1] * B[1][1] + A[0][2] * B[2][1];
  printf("thread a : %d\n", C[0][1]);           
  thread_yield();
  C[0][2] = A[0][0] * B[0][2] + A[0][1] * B[1][2] + A[0][2] * B[2][2];
  printf("thread a: %d\n", C[0][2]);
  thread_yield();

  printf("thread_a: exit after %d\n", a_n);
  current_thread->state = FREE;
  thread_schedule();
}

void 
thread_b(void)
{
  printf("thread_b started\n");
  b_started = 1;

  while(a_started == 0 || c_started == 0)
    thread_yield();
  
  // multiply(1, 'b');
  C[1][0] = A[1][0] * B[0][0] + A[1][1] * B[1][0] + A[1][2] * B[2][0];
  printf("thread b: %d\n", C[1][0]);
  thread_yield();
  C[1][1] = A[1][0] * B[0][1] + A[1][1] * B[1][1] + A[1][2] * B[2][1];
  printf("thread b: %d\n", C[1][1]);
  thread_yield();
  C[1][2] = A[1][0] * B[0][2] + A[1][1] * B[1][2] + A[1][2] * B[2][2];
  printf("thread b: %d\n", C[1][2]);
  thread_yield();

  printf("thread_b: exit after %d\n", b_n);
  current_thread->state = FREE;
  thread_schedule();
}

void 
thread_c(void)
{
  printf("thread_c started\n");
  c_started = 1;

  while(a_started == 0 || b_started == 0)
    thread_yield();
  
  // multiply(2,'c');
  // thread_yield();
  C[2][0] = A[2][0] * B[0][0] + A[2][1] * B[1][0] + A[2][2] * B[2][0];
  printf("thread c: %d\n", C[2][0]);
  thread_yield();
  C[2][1] = A[2][0] * B[0][1] + A[2][1] * B[1][1] + A[2][2] * B[2][1];
  printf("thread c: %d\n", C[2][1]);
  thread_yield();
  C[2][2] = A[2][0] * B[0][2] + A[2][1] * B[1][2] + A[2][2] * B[2][2];
  printf("thread c: %d\n", C[2][2]);
  thread_yield();

  printf("thread_c: exit after %d\n", c_n);
  current_thread->state = FREE;
  thread_schedule();
}

int 
main(int argc, char *argv[]) 
{
  a_started = b_started = c_started = 0;
  a_n = b_n = c_n = 0;
  thread_init();
  thread_create(thread_a);
  thread_create(thread_b);
  thread_create(thread_c);
  thread_schedule();

  exit(0);
}

```

Code for uthread_switch.S

``` cpp
	.text

	/*
         * save the old thread's registers,
         * restore the new thread's registers.
         */

	.globl thread_switch
thread_switch:
	/* YOUR CODE HERE */
	sd ra, 0(a0)
	sd sp, 8(a0)
	sd s0, 16(a0)
	sd s1, 24(a0)
	sd s2, 32(a0)
	sd s3, 40(a0)
	sd s4, 48(a0)
	sd s5, 56(a0)
	sd s6, 64(a0)
	sd s7, 72(a0)
	sd s8, 80(a0)
	sd s9, 88(a0)
	sd s10, 96(a0)
	sd s11, 104(a0)

	ld ra, 0(a1)
	ld sp, 8(a1)
	ld s0, 16(a1)
	ld s1, 24(a1)
	ld s2, 32(a1)
	ld s3, 40(a1)
	ld s4, 48(a1)
	ld s5, 56(a1)
	ld s6, 64(a1)
	ld s7, 72(a1)
	ld s8, 80(a1)
	ld s9, 88(a1)
	ld s10, 96(a1)
	ld s11, 104(a1)

	ret    /* return to ra */

```


--- 
