#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/dmesg.h"
#include "user/user.h"

static int
is_number(char *s)
{
  int i;

  if(s[0] == 0)
    return 0;

  for(i = 0; s[i]; i++){
    if(s[i] < '0' || s[i] > '9')
      return 0;
  }

  return 1;
}

static void
usage(void)
{
  printf("usage:\n");
  printf("  logctl off\n");
  printf("  logctl all [ticks]\n");
  printf("  logctl syscall [ticks]\n");
  printf("  logctl intr [ticks]\n");
  printf("  logctl proc [ticks]\n");
  printf("  logctl exec [ticks]\n");
  printf("  logctl syscall proc exec [ticks]\n");
}

int
main(int argc, char *argv[])
{
  int mask = 0;
  int ticks = 0;
  int old;
  int i;

  if(argc < 2){
    usage();
    exit(1);
  }

  for(i = 1; i < argc; i++){
    if(strcmp(argv[i], "off") == 0){
      mask = 0;
    } 
    else if(strcmp(argv[i], "all") == 0){
      mask |= DMSG_LOG_ALL;
    } 
    else if(strcmp(argv[i], "syscall") == 0){
      mask |= DMSG_LOG_SYSCALL;
    } 
    else if(strcmp(argv[i], "intr") == 0){
      mask |= DMSG_LOG_INTR;
    } 
    else if(strcmp(argv[i], "proc") == 0){
      mask |= DMSG_LOG_PROC;
    } 
    else if(strcmp(argv[i], "exec") == 0){
      mask |= DMSG_LOG_EXEC;
    }
    else if(is_number(argv[i])){
      ticks = atoi(argv[i]);
    } 
    else{
      usage();
      exit(1);
    }
  }

  old = logctl(mask, ticks);
  if(old < 0){
    printf("logctl: syscall failed\n");
    exit(1);
  }

  printf("old mask = %d, new mask = %d", old, mask);
  if(ticks > 0)
    printf(", duration = %d ticks", ticks);
  printf("\n");

  exit(0);
}