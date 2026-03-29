#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

static char
to_hex(int x)
{
  if(x < 10)
    return '0' + x;
  return 'A' + (x - 10);
}

int
main(int argc, char *argv[])
{
  int n;
  int fd;
  int i;
  int r;
  char *path;
  uchar buf[128];

  if(argc != 3){
    fprintf(2, "Usage: hexdump n file\n");
    exit(1);
  }

  n = atoi(argv[1]);
  path = argv[2];

  if(n < 0){
    fprintf(2, "Invalid length\n");
    exit(1);
  }

  fd = open(path, O_RDONLY);
  if(fd < 0){
    fprintf(2, "Open error\n");
    exit(1);
  }

  i = 0;
  while(i < n){
    int want = sizeof(buf);
    if(want > n - i)
      want = n - i;

    r = read(fd, buf, want);
    if(r < 0){
      fprintf(2, "Read error\n");
      close(fd);
      exit(1);
    }
    if(r == 0)
      break;

    for(int j = 0; j < r; j++){
      uchar b = buf[j];
      printf("%c%c", to_hex((b >> 4) & 15), to_hex(b & 15));
      if(i + j + 1 < n)
        printf(" ");
    }

    i += r;
  }

  printf("\n");
  close(fd);
  exit(0);
}