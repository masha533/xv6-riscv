#include "kernel/types.h"
#include "kernel/procinfo.h"
#include "user/user.h"

int
main(void)
{
  const char *states[] = {
    "UNUSED",
    "USED",
    "SLEEPING",
    "RUNNABLE",
    "RUNNING",
    "ZOMBIE"
  };
  int lim = ps_listinfo(0, 0);
  if(lim < 0){
    fprintf(2, "ps: ps_listinfo failed\n");
    exit(1);
  }
  if(lim == 0){
    exit(0);
  }
  while(1){
    struct procinfo *pinfo = malloc(lim * sizeof(struct procinfo));
    if(pinfo == 0){
      fprintf(2, "ps: malloc failed\n");
      exit(1);
    }
    int cnt = ps_listinfo(pinfo, lim);
    if(cnt < 0){
      fprintf(2, "ps: ps_listinfo failed\n");
      free(pinfo);
      exit(1);
    }
    if(cnt > lim){
      free(pinfo);
      lim = cnt;
      continue;
    }
    printf("pid\tname\tstate\tppid\tpname\n");
    for(int i = 0; i < cnt; i++){
      char *pname = "-";
      for(int j = 0; j < cnt; j++){
        if(pinfo[j].pid == pinfo[i].parent_pid){
          pname = pinfo[j].name;
          break;
        }
      }
      printf("%d\t%s\t%s\t%d\t%s\n", pinfo[i].pid, pinfo[i].name, states[pinfo[i].state], pinfo[i].parent_pid, pname);
    }
    free(pinfo);
    break;
  }
  exit(0);
}