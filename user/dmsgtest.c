#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid;

  printf("dmsgtest: start pid=%d\n", getpid());

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid == 0){
    printf("child: pid=%d\n", getpid());

    for(int i = 0; i < 5; i++){
      printf("child: i=%d\n", i);
      pause(5);
    }

    exit(0);
  } 
  else{
    printf("parent: pid=%d child=%d\n", getpid(), pid);

    for(int i = 0; i < 5; i++){
      printf("parent: i=%d\n", i);
      pause(5);
    }

    wait(0);
  }

  printf("dmsgtest: end pid=%d\n", getpid());
  exit(0);
}