#include "kernel/types.h"
#include "kernel/procinfo.h"
#include "user/user.h"

int
main(void)
{
  int passed = 0;
  int total = 5;
  int n = ps_listinfo(0, 0);
  if(n < 0){
    fprintf(2,"(fail) count-only test: %d\n", n);
    exit(1);
  }
  printf("(pass) count-only test: %d\n", n);
  passed++;

  int cap;
  if(n > 0){
    cap = n + 4; 
  } 
  else{
    cap = 1;
  }
  struct procinfo *buf = malloc(cap * sizeof(struct procinfo));
  if(buf == 0){
    fprintf(2, "(fail) malloc failed\n");
    exit(1);
  }
  int cnt = ps_listinfo(buf, cap);
  if(cnt < 0 || cnt > cap){
    fprintf(2, "(fail) normal call test: %d\n", cnt);
    free(buf);
    exit(1);
  }
  printf("(pass) normal call test: %d\n", cnt);
  passed++;

  int small = ps_listinfo(buf, 1);
  if(small <= 1){
    fprintf(2, "(fail) small-buffer test: %d\n", small);
    free(buf);
    exit(1);
  }
  printf("(pass) small-buffer test: %d\n", small);
  passed++;

  int bad = ps_listinfo((struct procinfo *)1, cap);
  if(bad >= 0){
    fprintf(2, "(fail) bad-address test: %d\n", bad);
    free(buf);
    exit(1);
  }
  printf("(pass) bad-address test: %d\n", bad);
  passed++; 

  int n1 = ps_listinfo(0, 0);
  int n2 = ps_listinfo(0, -7);
  if(n1 != n2){
    fprintf(2, "(fail) null-ignore-lim test: %d vs %d\n", n1, n2);
    exit(1);
  }
  printf("(pass) null-ignore-lim test: %d\n", n2);
  passed++;

  free(buf);

  printf("Passed %d/%d tests\n", passed, total);
  exit(0);
}