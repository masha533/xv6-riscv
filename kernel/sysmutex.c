#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "proc.h"
#include "file.h"
#include "defs.h"
#include "mutex.h"

uint64
sys_mutex(void)
{
  struct file *f;
  int fd;

  if(mutexalloc(&f) < 0)
    return -1;

  if((fd = fdalloc(f)) < 0){
    fileclose(f);
    return -1;
  }

  return fd;
}

uint64
sys_mutex_lock(void)
{
  struct file *f;

  if(argfd(0, 0, &f) < 0)
    return -1;

  return mutexlock(f);
}

uint64
sys_mutex_unlock(void)
{
  struct file *f;

  if(argfd(0, 0, &f) < 0)
    return -1;

  return mutexunlock(f);
}