#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

#define USER_DMSG_SIZE (DMSG_BUF_PAGES * 4096 + 1)

static char buf[USER_DMSG_SIZE];

int
main(int argc, char *argv[])
{
  int n;

  n = dmesg(buf, sizeof(buf));
  if(n < 0){
    printf("dmesg: syscall failed\n");
    exit(1);
  }

  printf("%s", buf);

  exit(0);
}