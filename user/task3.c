#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUF_SIZE 4096 
int
main(int argc, char *argv[])
{
  int pipefd[2];
  if(pipe(pipefd) < 0){
    printf("pipe failed\n");
    exit(1);
  }

  int pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid == 0){
    close(pipefd[1]);

    char buf[BUF_SIZE];
    int r;
    while((r = read(pipefd[0], buf, sizeof(buf))) > 0){
      int off = 0;
      while(off < r){
        int n = write(1, buf + off, r - off); 
        if(n < 0){
          printf("write failed\n");
          close(pipefd[0]);
          exit(1);
        }
        off += n;
      }
    }

    if(r < 0){
      printf("read failed\n");
      close(pipefd[0]);
      exit(1);
    }

    close(pipefd[0]);
    exit(0);
  }
  close(pipefd[0]);

  for(int i = 1; i < argc; i++){
    int len = strlen(argv[i]);
    int off = 0;
    while(off < len){
      int n = write(pipefd[1], argv[i] + off, len - off);
      if(n < 0){
        printf("write failed\n");
        close(pipefd[1]);
        wait(0);
        exit(1);
      }
      off += n;
    }
    if(write(pipefd[1], "\n", 1) != 1){
      printf("write newline failed\n");
      close(pipefd[1]);
      wait(0);
      exit(1);
    }
  }

  close(pipefd[1]);

  int status = 0;
  wait(&status);
  exit(0);
}