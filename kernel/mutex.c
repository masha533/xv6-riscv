#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"
#include "mutex.h"
int
mutexalloc(struct file **f)
{
  struct file *fp;
  struct sleeplock *lk;

  if((fp = filealloc()) == 0)
    return -1;

  lk = (struct sleeplock *)kalloc();
  if(lk == 0){
    fileclose(fp);
    return -1;
  }

  initsleeplock(lk, "mutex");

  fp->type = FD_MUTEX;
  fp->readable = 0;
  fp->writable = 0;
  fp->pipe = 0;
  fp->ip = 0;
  fp->mutex = lk;
  fp->off = 0;
  fp->major = 0;

  *f = fp;
  return 0;
}

void
mutexclose(struct sleeplock *lk)
{
  if(lk == 0)
    panic("mutexclose");
    
  kfree((char *)lk);
}

int
mutexlock(struct file *f)
{
  if(f == 0 || f->type != FD_MUTEX || f->mutex == 0)
    return -1;
  if(holdingsleep(f->mutex))
    return -1;

  acquiresleep(f->mutex);
  return 0;
}

int
mutexunlock(struct file *f)
{
  if(f == 0 || f->type != FD_MUTEX || f->mutex == 0)
    return -1;

  if(!holdingsleep(f->mutex))
    return -1;

  releasesleep(f->mutex);
  return 0;
}