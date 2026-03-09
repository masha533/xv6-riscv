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
    if(close(pipefd[0]) < 0){
      fprintf(2, "close failed\n");
    }
    if(close(pipefd[1]) < 0){
      fprintf(2, "close failed\n");
    }
    fprintf(2, "fork failed\n");
    exit(1);
  }

  if(pid == 0){
    if(close(pipefd[1]) < 0){
      fprintf(2, "close write end failed\n");
      exit(1);
    }

    if(close(0) < 0){
      fprintf(2, "close stdin failed\n");
      exit(1);
    }
    if(dup(pipefd[0]) != 0){
      fprintf(2, "dup failed\n");
      close(pipefd[0]);
      exit(1);
    }
    if(close(pipefd[0]) < 0){
      fprintf(2, "close failed\n");
      exit(1);
    }

    char *wargv[] = {"/wc", 0};
    exec("/wc", wargv);

    fprintf(2, "exec /wc failed\n");
    exit(1);
  }

  if(close(pipefd[0]) < 0){
    fprintf(2, "close failed\n");
    kill(pid);
    wait(0);
    exit(1);
  }

  for(int i = 1; i < argc; i++){
    int len = strlen(argv[i]);
    int off = 0;
    while(off < len){
      int n = write(pipefd[1], argv[i] + off, len - off);
      if(n < 0){
        fprintf(2, "write failed\n");
        if(close(pipefd[1]) < 0){
          fprintf(2, "close write end failed\n");
          kill(pid);
        }
        wait(0);
        exit(1);
      }
      off += n;
    }
    if(write(pipefd[1], "\n", 1) != 1){
      fprintf(2, "write newline failed\n");
      if (close(pipefd[1]) < 0){
        fprintf(2, "close write end failed\n");
        kill(pid);
      }
      wait(0);
      exit(1);
    }
  }

  if(close(pipefd[1]) < 0){
    fprintf(2, "close write end failed\n");
    kill(pid);
    wait(0);
    exit(1);
  }

  int status = 0;
  if(wait(&status) < 0){
    fprintf(2, "wait failed\n");
    exit(1);
  }
  exit(0);
}