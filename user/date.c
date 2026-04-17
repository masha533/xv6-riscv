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
static void
divmod(long long a, long long b, long long *q, long long *r)
{
  *q = a / b;
  *r = a % b;
  if (*r < 0) {
    *r += b;
    (*q)--;
  }
}

int
main(int argc, char **argv)
{
  static int mdays[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  long long ns = (long long)rtcdate();
  long long sec, nsec64;
  divmod(ns, 1000000000ll, &sec, &nsec64);

  long long days, sec_in_day;
  divmod(sec, 86400ll, &days, &sec_in_day);

  int year = 1970;
  while (days >= 146097) {
    days -= 146097;
    year += 400;
  }
  while (days < 0) {
    days += 146097;
    year -= 400;
  }
  while (1) {
    int ydays = is_leap(year) ? 366 : 365;
    if (days >= ydays) {
      days -= ydays;
      year++;
    } 
    else {
      break;
    }
  }

  int month = 0;
  while (month < 12) {
    int mday = mdays[month];
    if (month == 1 && is_leap(year))
      mday = 29;

    if (days >= mday) {
      days -= mday;
      month++;
    } 
    else {
      break;
    }
  }

  int day = (int)days + 1;

  int hour = (int)(sec_in_day / 3600);
  sec_in_day %= 3600;

  int minute = (int)(sec_in_day / 60);
  int second = (int)(sec_in_day % 60);

  uint32 nsec = (uint32)nsec64;

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