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
    fprintf(stderr, "pipe failed\n");
    exit(1);
  }

  int pid = fork();
  if(pid < 0){
    fprintf(stderr, "fork failed\n");
    if(close(pipefd[0]) < 0){
      fprintf(stderr, "close failed\n");
    }
    if(close(pipefd[1]) < 0){
      fprintf(stderr, "close failed\n");
    }
    exit(1);
  }

  if(pid == 0){
    if(close(pipefd[1]) < 0){
      fprintf(stderr, "close write end failed\n");
      exit(1);
    }

    char buf[BUF_SIZE];
    ssize_t r;
    while((r = read(pipefd[0], buf, sizeof(buf))) > 0){
      ssize_t off = 0;
      while(off < r){
        ssize_t n = write(1, buf + off, r - off); 
        if(n < 0){
          fprintf(stderr, "write failed\n");
          if(close(pipefd[0]) < 0){
            fprintf(stderr, "close read end failed\n");
          }
          exit(1);
        }
        off += n;
      }
    }

    if(r < 0){
      fprintf(stderr, "read failed\n");
      if(close(pipefd[0]) < 0){
        fprintf(stderr, "close read end failed\n");
      }
      exit(1);
    }

    if(close(pipefd[0]) < 0){
      fprintf(stderr, "close failed\n");
      exit(1);
    }
    exit(0);
  }
  if(close(pipefd[0]) < 0){
    fprintf(stderr, "close failed\n");
    exit(1);
  }

  for(int i = 1; i < argc; i++){
    int len = strlen(argv[i]);
    ssize_t off = 0;
    while(off < len){
      ssize_t n = write(pipefd[1], argv[i] + off, len - off);
      if(n < 0){
        fprintf(stderr, "write failed\n");
        if(close(pipefd[1]) < 0){
          fprintf(stderr, "close write end failed\n");
          exit(1);
        }
        wait(0);
        exit(1);
      }
      off += n;
    }
    if(write(pipefd[1], "\n", 1) != 1){
      fprintf(stderr, "write newline failed\n");
      if(close(pipefd[1]) < 0){
        fprintf(stderr, "close write end failed\n");
        exit(1);
      }
      wait(0);
      exit(1);
    }
  }

  if(close(pipefd[1]) < 0){
    fprintf(stderr, "close write end failed\n");
    exit(1);
  }

  int status = 0;
  if(wait(&status) < 0){
    fprintf(stderr, "wait failed\n");
    exit(1);
  }
  exit(0);
}