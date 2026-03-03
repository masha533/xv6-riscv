#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pipefd[2];
  if(pipe(pipefd) < 0){
    fprintf(2, "pipe failed\n");
    exit(1);
  }
  
  int pid = fork();
  if(pid < 0){
    fprintf(2, "fork failed\n");
    exit(1);
  }

  if(pid == 0){
    close(pipefd[1]);

    close(0);
    if(dup(pipefd[0]) != 0){
      fprintf(2, "dup failed\n");
      exit(1);
    }
    close(pipefd[0]);

    char *wargv[] = {"/wc", 0};
    exec("/wc", wargv);

    fprintf(2, "exec /wc failed\n");
    exit(1);
  }

  close(pipefd[0]);

  for(int i = 1; i < argc; i++){
    int len = strlen(argv[i]);
    int off = 0;
    while(off < len){
      int n = write(pipefd[1], argv[i] + off, len - off);
      if(n < 0){
        fprintf(2, "write failed\n");
        close(pipefd[1]);
        exit(1);
      }
      off += n;
    }
    if(write(pipefd[1], "\n", 1) != 1){
      fprintf(2, "write newline failed\n");
      close(pipefd[1]);
      exit(1);
    }
  }

  close(pipefd[1]);

  int status = 0;
  wait(&status);
  exit(0);
}