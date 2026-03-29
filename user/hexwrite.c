#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

static int
hex_value(char c)
{
  if('0' <= c && c <= '9')
    return c - '0';
  if('a' <= c && c <= 'f')
    return c - 'a' + 10;
  if('A' <= c && c <= 'F')
    return c - 'A' + 10;
  return -1;
}

int
main(int argc, char *argv[])
{
  char *hex;
  char *path;
  int len;
  int fd;
  int i;
  int hi, lo;
  uchar buf[128];
  int n;
  int w;

  if(argc != 3){
    fprintf(2, "Usage: hexwrite HEX file\n");
    exit(1);
  }

  hex = argv[1];
  path = argv[2];
  len = strlen(hex);

  if(len % 2 != 0){
    fprintf(2, "Hex string length must be even\n");
    exit(1);
  }

  n = len / 2;
  if(n > sizeof(buf)){
    fprintf(2, "Too many bytes\n");
    exit(1);
  }

  for(i = 0; i < n; i++){
    hi = hex_value(hex[2 * i]);
    lo = hex_value(hex[2 * i + 1]);

    if(hi < 0 || lo < 0){
      fprintf(2, "Invalid hex string\n");
      exit(1);
    }

    buf[i] = (hi << 4) | lo;
  }

  fd = open(path, O_WRONLY);
  if(fd < 0){
    fprintf(2, "Open error\n");
    exit(1);
  }

  w = write(fd, buf, n);
  if(w != n){
    fprintf(2, "Write error\n");
    close(fd);
    exit(1);
  }

  close(fd);
  exit(0);
}