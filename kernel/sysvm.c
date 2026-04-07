#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_pgtprint(void)
{
  struct proc *p = myproc();
  vmprint(p->pagetable);
  return 0;
}

uint64
sys_pgclearflags(void)
{
  uint64 addr;
  int len;
  int flags;
  struct proc *p = myproc();

  argaddr(0, &addr);
  argint(1, &len);
  argint(2, &flags);

  return pgclearflags(p->pagetable, addr, len, flags);
}

uint64
sys_pgcheckflags(void)
{
  uint64 addr;
  int len;
  int flags;
  struct proc *p = myproc();

  argaddr(0, &addr);
  argint(1, &len);
  argint(2, &flags);

  return pgcheckflags(p->pagetable, addr, len, flags);
}