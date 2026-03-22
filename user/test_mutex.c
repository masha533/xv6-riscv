#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
static void
print_one_locked(int argi, char c, int mfd)
{
  if(mutex_lock(mfd) < 0){
    printf("pid %d: mutex_lock failed\n", getpid());
    exit(1);
  }

  printf("%d", getpid());
  pause(1);
  printf(": arg %d", argi);
  pause(1);
  printf(", char '%c'\n", c);
  pause(1);

  if(mutex_unlock(mfd) < 0){
    printf("pid %d: mutex_unlock failed\n", getpid());
    exit(1);
  }
}

static void
dump_args_locked(int argc, char *argv[], int mfd)
{
  for(int i = 1; i < argc; i++){
    for(int j = 0; argv[i][j] != 0; j++){
      print_one_locked(i, argv[i][j], mfd);
    }
  }
}

static void
demo_with_mutex(int argc, char *argv[])
{
  int pid;
  int mfd;

  if(argc < 2){
    printf("wrong number of arguments\n");
    return;
  }

  mfd = mutex();
  if(mfd < 0){
    printf("mutex create failed\n");
    exit(1);
  }

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    close(mfd);
    exit(1);
  }

  if(pid == 0){
    dump_args_locked(argc, argv, mfd);
    close(mfd);
    exit(0);
  } 
  else{
    dump_args_locked(argc, argv, mfd);
    wait(0);
    close(mfd);
  }
}

static void
test_rw_fstat(void)
{
  int m;
  char x = 'A';
  struct stat st;
  m = mutex();
  printf("mutex() = %d\n", m);
  printf("read(m)  = %d\n", read(m, &x, 1));
  printf("write(m) = %d\n", write(m, &x, 1));
  printf("fstat(m) = %d\n", fstat(m, &st));
  printf("close(m) = %d\n", close(m));
}

static void
test_close_by_owner(void)
{
  int m, pid;
  m = mutex();
  if(m < 0){
    printf("mutex create failed\n");
    exit(1);
  }

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    close(m);
    exit(1);
  }

  if(pid == 0){
    int r;
    pause(30);
    r = mutex_lock(m);
    printf("child: mutex_lock after parent close = %d\n", r);
    if(r == 0){
      printf("child: mutex_unlock = %d\n", mutex_unlock(m));
    }
    printf("child: close = %d\n", close(m));
    exit(0);
  } 
  else{
    printf("parent: mutex_lock = %d\n", mutex_lock(m));
    printf("parent: close while owning = %d\n", close(m));
    wait(0);
  }
}

static void
test_close_by_other(void)
{
  int m, pid;
  m = mutex();
  if(m < 0){
    printf("mutex create failed\n");
    exit(1);
  }

  printf("parent: mutex_lock = %d\n", mutex_lock(m));

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    close(m);
    exit(1);
  }

  if(pid == 0){
    pause(10);
    printf("child: close = %d\n", close(m));
    exit(0);
  } 
  else{
    pause(30);
    printf("parent: mutex_unlock after child close = %d\n", mutex_unlock(m));
    printf("parent: close = %d\n", close(m));
    wait(0);
  }
}

static void
test_unlock_by_other(void)
{
  int m, pid;
  m = mutex();
  if(m < 0){
    printf("mutex create failed\n");
    exit(1);
  }

  printf("parent: mutex_lock = %d\n", mutex_lock(m));

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    close(m);
    exit(1);
  }

  if(pid == 0){
    pause(10);
    printf("child: mutex_unlock = %d\n", mutex_unlock(m));
    printf("child: close = %d\n", close(m));
    exit(0);
  } 
  else{
    pause(30);
    printf("parent: mutex_unlock = %d\n", mutex_unlock(m));
    printf("parent: close = %d\n", close(m));
    wait(0);
  }
}

static void
test_exit_with_locked_mutex(void)
{
  int m, pid;
  m = mutex();
  if(m < 0){
    printf("mutex create failed\n");
    exit(1);
  }

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    close(m);
    exit(1);
  }

  if(pid == 0){
    printf("child: mutex_lock = %d\n", mutex_lock(m));
    printf("child: exit without unlock/close\n");
    exit(0);
  } 
  else{
    int r;
    wait(0);
    r = mutex_lock(m);
    printf("parent: mutex_lock after child exit = %d\n", r);
    if(r == 0){
      printf("parent: mutex_unlock = %d\n", mutex_unlock(m));
    }
    printf("parent: close = %d\n", close(m));
  }
}

int
main(int argc, char *argv[])
{
  demo_with_mutex(argc, argv);
  test_rw_fstat();
  test_close_by_owner();
  test_close_by_other();
  test_unlock_by_other();
  test_exit_with_locked_mutex();
  exit(0);
}