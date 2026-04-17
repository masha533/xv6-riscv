#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PGSIZE   4096
#define PTE_A    (1L << 6)
#define PTE_D    (1L << 7)
#define HEAP_LEN (4 * PGSIZE)
#define STACK_OFF 123

volatile int global_var = 100;
volatile int global_arr[16];

static char *heap;
static volatile int sink;
static void
fail(const char *msg)
{
  printf("ERROR: %s\n", msg);
  exit(1);
}

static void
show_flags(const char *name, void *addr, int len)
{
  int a = pgcheckflags(addr, len, PTE_A);
  int d = pgcheckflags(addr, len, PTE_D);
  int ad = pgcheckflags(addr, len, PTE_A | PTE_D);
  if(a < 0 || d < 0 || ad < 0)
    fail("pgcheckflags failed");

  printf("%s: addr=%p len=%d A=%d D=%d AD=%d\n", name, addr, len, a, d, ad);
}

static void
clear_and_show(const char *name, void *addr, int len)
{
  int rc;

  printf("%s\n", name);
  printf("  before: ");
  show_flags("", addr, len);

  rc = pgclearflags(addr, len, PTE_A | PTE_D);
  printf("  action: clear(A|D) -> %d\n", rc);
  if(rc < 0)
    fail("pgclearflags failed");

  printf("  after : ");
  show_flags("", addr, len);
}

int
main(void)
{
  volatile int stack_var = 10;
  volatile char stack_arr[512];

  printf("\nSTART\n");
  pgtprint();

  printf("\nALLOCATE HEAP\n");
  heap = sbrk(HEAP_LEN);
  if(heap == (char *)-1){
    printf("sbrk failed\n");
    exit(1);
  }
  pgtprint();

  global_arr[5] = 11;
  stack_arr[STACK_OFF] = 7;

  heap[0] = 1;
  heap[5000] = 2;
  heap[9000] = 3;
  heap[13000] = 4;

  printf("\nINITIAL FLAGS\n");
  show_flags("global_var", (void *)&global_var, sizeof(global_var));
  show_flags("global_arr[5]", (void *)&global_arr[5], sizeof(global_arr[5]));
  show_flags("stack_var", (void *)&stack_var, sizeof(stack_var));
  show_flags("stack_arr[STACK_OFF]", (void *)&stack_arr[STACK_OFF], 1);
  show_flags("heap[0]", (void *)&heap[0], 1);
  show_flags("heap[5000]", (void *)&heap[5000], 1);
  show_flags("heap[9000]", (void *)&heap[9000], 1);
  show_flags("heap[13000]", (void *)&heap[13000], 1);
  show_flags("heap whole", (void *)heap, HEAP_LEN);

  printf("\nCLEAR FLAGS: BEFORE / ACTION / AFTER\n");
  clear_and_show("global_var", (void *)&global_var, sizeof(global_var));
  clear_and_show("global_arr[5]", (void *)&global_arr[5], sizeof(global_arr[5]));
  clear_and_show("stack_var", (void *)&stack_var, sizeof(stack_var));
  clear_and_show("stack_arr[STACK_OFF]", (void *)&stack_arr[STACK_OFF], 1);
  clear_and_show("heap[0]", (void *)&heap[0], 1);
  clear_and_show("heap[5000]", (void *)&heap[5000], 1);
  clear_and_show("heap[9000]", (void *)&heap[9000], 1);
  clear_and_show("heap[13000]", (void *)&heap[13000], 1);
  clear_and_show("heap whole", (void *)heap, HEAP_LEN);

  printf("\nPAGETABLE AFTER CLEAR\n");
  pgtprint();

  printf("\nREAD: BEFORE\n");
  show_flags("global_var", (void *)&global_var, sizeof(global_var));
  show_flags("global_arr[5]", (void *)&global_arr[5], sizeof(global_arr[5]));
  show_flags("stack_var", (void *)&stack_var, sizeof(stack_var));
  show_flags("stack_arr[STACK_OFF]", (void *)&stack_arr[STACK_OFF], 1);
  show_flags("heap[0]", (void *)&heap[0], 1);
  show_flags("heap[5000]", (void *)&heap[5000], 1);
  show_flags("heap[9000]", (void *)&heap[9000], 1);
  show_flags("heap[13000]", (void *)&heap[13000], 1);
  show_flags("heap whole", (void *)heap, HEAP_LEN);

  sink += global_var;
  sink += global_arr[5];
  sink += stack_var;
  sink += stack_arr[STACK_OFF];
  sink += heap[0];
  sink += heap[5000];
  sink += heap[9000];
  sink += heap[13000];

  printf("READ: ACTION -> read tracked data, sink=%d\n", sink);

  printf("READ: AFTER\n");
  show_flags("global_var", (void *)&global_var, sizeof(global_var));
  show_flags("global_arr[5]", (void *)&global_arr[5], sizeof(global_arr[5]));
  show_flags("stack_var", (void *)&stack_var, sizeof(stack_var));
  show_flags("stack_arr[STACK_OFF]", (void *)&stack_arr[STACK_OFF], 1);
  show_flags("heap[0]", (void *)&heap[0], 1);
  show_flags("heap[5000]", (void *)&heap[5000], 1);
  show_flags("heap[9000]", (void *)&heap[9000], 1);
  show_flags("heap[13000]", (void *)&heap[13000], 1);
  show_flags("heap whole", (void *)heap, HEAP_LEN);

  printf("\nPAGETABLE AFTER READ\n");
  pgtprint();

  printf("\nCLEAR FLAGS BEFORE WRITE\n");
  if(pgclearflags((void *)&global_var, sizeof(global_var), PTE_A | PTE_D) < 0)
    fail("clear global_var failed");
  if(pgclearflags((void *)&global_arr[5], sizeof(global_arr[5]), PTE_A | PTE_D) < 0)
    fail("clear global_arr[5] failed");
  if(pgclearflags((void *)&stack_var, sizeof(stack_var), PTE_A | PTE_D) < 0)
    fail("clear stack_var failed");
  if(pgclearflags((void *)&stack_arr[STACK_OFF], 1, PTE_A | PTE_D) < 0)
    fail("clear stack_arr failed");
  if(pgclearflags((void *)heap, HEAP_LEN, PTE_A | PTE_D) < 0)
    fail("clear heap failed");

  show_flags("global_var", (void *)&global_var, sizeof(global_var));
  show_flags("global_arr[5]", (void *)&global_arr[5], sizeof(global_arr[5]));
  show_flags("stack_var", (void *)&stack_var, sizeof(stack_var));
  show_flags("stack_arr[STACK_OFF]", (void *)&stack_arr[STACK_OFF], 1);
  show_flags("heap whole", (void *)heap, HEAP_LEN);

  printf("\nWRITE: BEFORE\n");
  show_flags("global_var", (void *)&global_var, sizeof(global_var));
  show_flags("global_arr[5]", (void *)&global_arr[5], sizeof(global_arr[5]));
  show_flags("stack_var", (void *)&stack_var, sizeof(stack_var));
  show_flags("stack_arr[STACK_OFF]", (void *)&stack_arr[STACK_OFF], 1);
  show_flags("heap[0]", (void *)&heap[0], 1);
  show_flags("heap[5000]", (void *)&heap[5000], 1);
  show_flags("heap[9000]", (void *)&heap[9000], 1);
  show_flags("heap[13000]", (void *)&heap[13000], 1);
  show_flags("heap whole", (void *)heap, HEAP_LEN);

  global_var += 1;
  global_arr[5] += 1;
  stack_var += 1;
  stack_arr[STACK_OFF] += 1;
  heap[0] += 1;
  heap[5000] += 1;
  heap[9000] += 1;
  heap[13000] += 1;

  printf("WRITE: ACTION -> write tracked data\n");

  printf("WRITE: AFTER\n");
  show_flags("global_var", (void *)&global_var, sizeof(global_var));
  show_flags("global_arr[5]", (void *)&global_arr[5], sizeof(global_arr[5]));
  show_flags("stack_var", (void *)&stack_var, sizeof(stack_var));
  show_flags("stack_arr[STACK_OFF]", (void *)&stack_arr[STACK_OFF], 1);
  show_flags("heap[0]", (void *)&heap[0], 1);
  show_flags("heap[5000]", (void *)&heap[5000], 1);
  show_flags("heap[9000]", (void *)&heap[9000], 1);
  show_flags("heap[13000]", (void *)&heap[13000], 1);
  show_flags("heap whole", (void *)heap, HEAP_LEN);

  printf("\nPAGETABLE AFTER WRITE\n");
  pgtprint();

  printf("\nFREE HEAP\n");
  if(sbrk(-HEAP_LEN) == (char *)-1){
    printf("sbrk shrink failed\n");
    exit(1);
  }
  pgtprint();

  exit(0);
}