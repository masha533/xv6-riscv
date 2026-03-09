#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  int pid = fork();
  if(pid < 0){
    fprintf(2, "fork failed\n");
    exit(1);
  }

  if(pid == 0){
    pause(120);
    exit(1);
  }

  printf("parent %d: child pid=%d\n", getpid(), pid);
  if (kill(pid) == -1){
    fprintf(2, "kill failed\n");
    exit(1);
  }

  int status = 0;
  int w = wait(&status);
  if(w < 0){
    fprintf(2, "wait failed\n");
    exit(1);
  }
  printf("child %d is done with %d status code\n", w, status);

  exit(0);
}