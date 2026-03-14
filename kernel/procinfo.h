#pragma once
struct procinfo
{
    int pid;
    char name[16];
    int state;
    int parent_pid;
};