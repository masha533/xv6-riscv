#ifndef XV6_MUTEX_H
#define XV6_MUTEX_H

struct file;
struct sleeplock;

int  mutexalloc(struct file **f);
void mutexclose(struct sleeplock *lk);
int  mutexlock(struct file *f);
int  mutexunlock(struct file *f);

#endif