#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static int
is_leap(int year)
{
  if (year % 400 == 0)
    return 1;
  if (year % 100 == 0)
    return 0;
  return (year % 4 == 0);
}

static void
print2(int x)
{
  printf("%d", x / 10);
  printf("%d", x % 10);
}

static void
print4(int x)
{
  print2(x / 100);
  print2(x % 100);
}

static void
print9(uint32 x)
{
  uint32 d = 100000000;
  while (d > 0) {
    printf("%d", (x / d) % 10);
    d /= 10;
  }
}

int
main(int argc, char **argv)
{
  static int mdays[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  uint64 ns = rtcdate();
  uint64 sec = ns / 1000000000ull;
  uint32 nsec = (uint32)(ns % 1000000000ull);

  int year = 1970;
  while (1) {
    int days = is_leap(year) ? 366 : 365;
    uint64 year_sec = (uint64)days * 86400ull;
    if (sec >= year_sec) {
      sec -= year_sec;
      year++;
    } 
    else {
      break;
    }
  }

  int month = 0;
  while (month < 12) {
    int days = mdays[month];
    if (month == 1 && is_leap(year))
      days = 29;

    uint64 month_sec = (uint64)days * 86400ull;
    if (sec >= month_sec){
      sec -= month_sec;
      month++;
    } 
    else {
      break;
    }
  }

  int day = (int)(sec / 86400ull) + 1;
  sec %= 86400ull;

  int hour = (int)(sec / 3600ull);
  sec %= 3600ull;

  int minute = (int)(sec / 60ull);
  int second = (int)(sec % 60ull);

  print4(year);
  printf("-");
  print2(month + 1);
  printf("-");
  print2(day);
  printf(" ");
  print2(hour);
  printf(":");
  print2(minute);
  printf(":");
  print2(second);
  printf(".");
  print9(nsec);
  printf("\n");

  exit(0);
}