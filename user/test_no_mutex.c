#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static void
print_one(int argi, char c)
{
  printf("%d", getpid());
  pause(1);
  printf(": arg %d", argi);
  pause(1);
  printf(", char '%c'\n", c);
  pause(1);
}

static void
dump_args(int argc, char *argv[])
{
  for(int i = 1; i < argc; i++){
    for(int j = 0; argv[i][j] != 0; j++){
      print_one(i, argv[i][j]);
    }
  }
}

int
main(int argc, char *argv[])
{
  int pid;

  if(argc < 2){
    printf("wrong number of arguments\n");
    exit(1);
  }

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid == 0){
    dump_args(argc, argv);
    exit(0);
  } 
  else{
    dump_args(argc, argv);
    wait(0);
  }

  exit(0);
}