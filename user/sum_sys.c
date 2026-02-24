#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(){
    char buf[512];
    int n = 0;
    while (n + 1 < 512)
    {
        char c;
        int r = read(0, &c, 1);
        if (r < 0){
            fprintf(2, "error: read failed\n");
            exit(1);
        }
        if (r == 0 || c == '\n'){
            break;
        }
        buf[n] = c;
        n++;
    }
    if (n + 1 == 512){
        fprintf(2, "error: input too long\n");
        exit(1);
    }
    buf[n] = '\0';
    if (n == 0){
        fprintf(2, "error: no input\n");
        exit(1);
    }
    printf("|%s|\n", buf);
    int space = -1;
    for (int i = 0; buf[i] != '\0'; i++){
        if (buf[i] == ' '){
            if(space == -1){
                space = i;
            }
            else {
                fprintf(2, "error: invalid input(only 1 space allowed)\n");
                exit(1);
            }
        }
    }
    if (space == -1){
        fprintf(2, "error: invalid input(no space)\n");
        exit(1);
    }
    if(space == 0){
        fprintf(2, "error: no first number\n");
        exit(1);
    }
    if (buf[space + 1] == '\0'){
        fprintf(2, "error: no second number\n");
        exit(1);
    }
    buf[space] = '\0';
    char *a = buf;
    char *b = buf + space + 1;
    int i = 0;
    if(a[i] == '-'){
        i++;
        if(a[i] == '\0'){
            fprintf(2, "error: invalid first number\n");
            exit(1);
        }
    }
    while (a[i] != '\0'){
        if(a[i] < '0' || a[i] > '9'){
            fprintf(2, "error: invalid first number\n");
            exit(1);
        }
        i++;
    }
    i = 0;
    if(b[i] == '-'){
        i++;
        if(b[i] == '\0'){
            fprintf(2, "error: invalid second number\n");
            exit(1);
        }
    }
    while (b[i] != '\0'){
        if(b[i] < '0' || b[i] > '9'){
            fprintf(2, "error: invalid second number\n");
            exit(1);
        }
        i++;
    }
    int sum = add(atoi(a), atoi(b));
    printf("%d\n", sum);
    exit(0);
}