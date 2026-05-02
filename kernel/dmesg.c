#include <stdarg.h>
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "dmesg.h"

#define DMSG_BUF_SIZE (DMSG_BUF_PAGES * PGSIZE)

extern struct spinlock tickslock;
extern uint ticks;

static char digits[] = "0123456789abcdef";

static struct{
  struct spinlock lock;

  char buf[DMSG_BUF_SIZE];
  int head;
  int tail;
  int full;
  int log_mask;
  uint log_until;
} dmsg;


static int
dmsg_len_locked(void)
{
  if(dmsg.full)
    return DMSG_BUF_SIZE;

  if(dmsg.tail >= dmsg.head)
    return dmsg.tail - dmsg.head;

  return DMSG_BUF_SIZE - dmsg.head + dmsg.tail;
}


static void
dmsg_write_char_locked(int c)
{
  dmsg.buf[dmsg.tail] = c;
  dmsg.tail++;

  if(dmsg.tail == DMSG_BUF_SIZE)
    dmsg.tail = 0;

  if(dmsg.full){
    dmsg.head++;
    if(dmsg.head == DMSG_BUF_SIZE)
      dmsg.head = 0;
  }

  if(dmsg.tail == dmsg.head)
    dmsg.full = 1;
}

void
dmsginit(void)
{
  initlock(&dmsg.lock, "dmesg");

  dmsg.head = 0;
  dmsg.tail = 0;
  dmsg.full = 0;

  dmsg.log_mask = 0;
  dmsg.log_until = 0;
}


static void
dmsg_printint_locked(long long xx, int base, int sign)
{
  char buf[32];
  int i;
  unsigned long long x;

  if(sign && xx < 0)
    x = -xx;
  else
    x = xx;

  i = 0;
  do{
    buf[i++] = digits[x % base];
  } while((x /= base) != 0);

  if(sign && xx < 0)
    buf[i++] = '-';

  while(--i >= 0)
    dmsg_write_char_locked(buf[i]);
}


static void
dmsg_printptr_locked(uint64 x)
{
  int i;

  dmsg_write_char_locked('0');
  dmsg_write_char_locked('x');

  for(i = 0; i < sizeof(uint64) * 2; i++, x <<= 4)
    dmsg_write_char_locked(digits[x >> (sizeof(uint64) * 8 - 4)]);
}


static void
dmsg_vprintf_locked(const char *fmt, va_list ap)
{
  int i, cx;
  int c0, c1, c2;
  char *s;

  if(fmt == 0)
    return;

  for(i = 0; (cx = fmt[i] & 0xff) != 0; i++){
    if(cx != '%'){
      dmsg_write_char_locked(cx);
      continue;
    }

    i++;

    c0 = fmt[i + 0] & 0xff;
    c1 = 0;
    c2 = 0;

    if(c0)
      c1 = fmt[i + 1] & 0xff;
    if(c1)
      c2 = fmt[i + 2] & 0xff;

    if(c0 == 'd'){
      dmsg_printint_locked(va_arg(ap, int), 10, 1);
    } 
    else if(c0 == 'l' && c1 == 'd'){
      dmsg_printint_locked(va_arg(ap, long), 10, 1);
      i += 1;
    }
    else if(c0 == 'l' && c1 == 'l' && c2 == 'd'){
      dmsg_printint_locked(va_arg(ap, long long), 10, 1);
      i += 2;
    } 
    else if(c0 == 'u'){
      dmsg_printint_locked(va_arg(ap, unsigned int), 10, 0);
    } 
    else if(c0 == 'l' && c1 == 'u'){
      dmsg_printint_locked(va_arg(ap, unsigned long), 10, 0);
      i += 1;
    } 
    else if(c0 == 'l' && c1 == 'l' && c2 == 'u'){
      dmsg_printint_locked(va_arg(ap, unsigned long long), 10, 0);
      i += 2;
    } 
    else if(c0 == 'x'){
      dmsg_printint_locked(va_arg(ap, unsigned int), 16, 0);
    } 
    else if(c0 == 'l' && c1 == 'x'){
      dmsg_printint_locked(va_arg(ap, unsigned long), 16, 0);
      i += 1;
    } 
    else if(c0 == 'l' && c1 == 'l' && c2 == 'x'){
      dmsg_printint_locked(va_arg(ap, unsigned long long), 16, 0);
      i += 2;
    } 
    else if(c0 == 'p'){
      dmsg_printptr_locked(va_arg(ap, uint64));
    } 
    else if(c0 == 'c'){
      dmsg_write_char_locked(va_arg(ap, int));
    }
    else if(c0 == 's'){
      s = va_arg(ap, char *);
      if(s == 0)
        s = "(null)";
      for(; *s; s++)
        dmsg_write_char_locked(*s);
    } 
    else if(c0 == '%'){
      dmsg_write_char_locked('%');
    } 
    else if(c0 == 0){
      break;
    } 
    else{
      dmsg_write_char_locked('%');
      dmsg_write_char_locked(c0);
    }
  }
}


void
pr_msg(const char *fmt, ...)
{
  va_list ap;
  uint now;

  acquire(&tickslock);
  now = ticks;
  release(&tickslock);

  acquire(&dmsg.lock);

  dmsg_write_char_locked('[');
  dmsg_printint_locked(now, 10, 0);
  dmsg_write_char_locked(']');
  dmsg_write_char_locked(' ');

  va_start(ap, fmt);
  dmsg_vprintf_locked(fmt, ap);
  va_end(ap);

  dmsg_write_char_locked('\n');

  release(&dmsg.lock);
}


int
dmsg_copyout(uint64 dst, int max)
{
  struct proc *p;
  int n;
  int i;
  int pos;
  char c;
  char zero = 0;

  if(max <= 0)
    return -1;

  p = myproc();

  acquire(&dmsg.lock);

  n = dmsg_len_locked();
  if(n > max - 1)
    n = max - 1;

  pos = dmsg.head;

  for(i = 0; i < n; i++){
    c = dmsg.buf[pos];

    if(copyout(p->pagetable, dst + i, &c, 1) < 0){
      release(&dmsg.lock);
      return -1;
    }

    pos++;
    if(pos == DMSG_BUF_SIZE)
      pos = 0;
  }

  if(copyout(p->pagetable, dst + n, &zero, 1) < 0){
    release(&dmsg.lock);
    return -1;
  }

  release(&dmsg.lock);

  return n;
}


int
dmsg_should_log(int class)
{
  uint now;
  int res;

  acquire(&tickslock);
  now = ticks;
  release(&tickslock);

  acquire(&dmsg.lock);

  if(dmsg.log_until != 0 && now >= dmsg.log_until){
    dmsg.log_mask = 0;
    dmsg.log_until = 0;
  }

  res = (dmsg.log_mask & class) != 0;

  release(&dmsg.lock);

  return res;
}


int
dmsg_logctl(int mask, int duration)
{
  uint now;
  int old;

  if(mask < 0)
    return -1;

  mask &= DMSG_LOG_ALL;

  acquire(&tickslock);
  now = ticks;
  release(&tickslock);

  acquire(&dmsg.lock);

  old = dmsg.log_mask;

  dmsg.log_mask = mask;

  if(mask == 0)
    dmsg.log_until = 0;
  else if(duration > 0)
    dmsg.log_until = now + duration;
  else
    dmsg.log_until = 0;

  release(&dmsg.lock);

  return old;
}