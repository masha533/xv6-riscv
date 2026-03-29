#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "riscv.h"
#include "defs.h"

static struct spinlock pseudo_lock;
static uint64 nullstat_count;
static uint urandom_seed = 1;

void
pseudoinit(void)
{
  initlock(&pseudo_lock, "pseudo");
  nullstat_count = 0;
  urandom_seed = 1;
}

static uint
lcg_next(void)
{
  urandom_seed = urandom_seed * 1664525 + 1013904223;
  return urandom_seed;
}

int
pseudoread(short minor, int user_dst, uint64 dst, int n)
{
  int i;
  char c;
  uint64 val;

  switch(minor){
  case M_NULL:
    return 0;

  case M_ZERO:
    c = 0;
    for(i = 0; i < n; i++){
      if(either_copyout(user_dst, dst + i, &c, 1) < 0)
        return -1;
    }
    return n;

  case M_URANDOM:
    for(i = 0; i < n; i++){
      acquire(&pseudo_lock);
      c = (char)(lcg_next() & 0xFF);
      release(&pseudo_lock);

      if(either_copyout(user_dst, dst + i, &c, 1) < 0)
        return -1;
    }
    return n;

  case M_NULLSTAT:
    if(n != sizeof(uint64))
      return -1;

    acquire(&pseudo_lock);
    val = nullstat_count;
    release(&pseudo_lock);

    if(either_copyout(user_dst, dst, (char *)&val, sizeof(uint64)) < 0)
      return -1;

    return sizeof(uint64);
  }

  return -1;
}

int
pseudowrite(short minor, int user_src, uint64 src, int n)
{
  uint new_seed;

  switch(minor){
  case M_NULL:
    return n;

  case M_ZERO:
    return -1;

  case M_URANDOM:
    if(n != sizeof(uint))
      return -1;

    if(either_copyin((char *)&new_seed, user_src, src, sizeof(uint)) < 0)
      return -1;

    acquire(&pseudo_lock);
    urandom_seed = new_seed;
    release(&pseudo_lock);

    return sizeof(uint);

  case M_NULLSTAT:
    acquire(&pseudo_lock);
    nullstat_count += n;
    release(&pseudo_lock);
    return n;
  }

  return -1;
}