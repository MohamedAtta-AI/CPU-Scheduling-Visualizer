#include <stdio.h>
#include <stdlib.h>

#define MAX_IDS 1000

typedef struct Process {
    char id[MAX_IDS];
    int arrive_time;
    int burst;
    int waiting_time;
    int turnaround_time;
    int return_time;
    int response_time;
    int priority;
    int completed;
} Process;

typedef int Quantum;

void initializeProcesses(Process p[], int len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        p[i].waiting_time = 0;
        p[i].return_time = 0;
        p[i].response_time = 0;
        p[i].completed = 0;
    }
}