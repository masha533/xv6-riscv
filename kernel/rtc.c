#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"

static struct spinlock rtc_lock;

#define ReadReg(addr) (*(volatile uint32 *)(addr))

void
rtcinit(void)
{
  initlock(&rtc_lock, "rtc");
}

static uint32
rtc_read_low(void)
{
  return ReadReg(RTC_LOW);
}

static uint32
rtc_read_high(void)
{
  return ReadReg(RTC_HIGH);
}

uint64
rtctime(void)
{
  uint32 low, high;
  acquire(&rtc_lock);
  low = rtc_read_low();  
  high = rtc_read_high();   
  release(&rtc_lock);

  return ((uint64)high << 32) | (uint64)low;
}