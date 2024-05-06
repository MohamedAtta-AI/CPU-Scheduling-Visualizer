#include <stdio.h>
#include "../data_structures/Process.c"
#include "../data_structures/LinkedList.c"


void fcfs(Process processes[], int n) {
    // Sort by arrival
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (processes[i].arrival_time > processes[j].arrival_time) {
                Process temp = processes[i];
                processes[i] = processes[j];
                processes[j] = temp;
            }
        }
    }

    int waiting_time[n], turnaround_time[n];
    int total_waiting = 0, total_turnaround = 0;
    for(int i = 0; i < n; i++){
        waiting_time[i]=0;
        for(int j = 0; j < i; j++){
            waiting_time[i] += processes[j].burst_time;
        }
        total_waiting += waiting_time[i];

        turnaround_time[i] = processes[i].burst_time + waiting_time[i]; 
        total_turnaround += turnaround_time[i];   
    }

    struct Node* gantt = NULL;
    int idle_time = 0;

    for (int i = 0; i < n; i++) {
        int prev_turnaround = (i == 0)? 0: turnaround_time[i-1];

        if (processes[i].arrival_time > prev_turnaround) {
            idle_time += processes[i].arrival_time - prev_turnaround;
            insertAtEnd(&gantt, prev_turnaround, processes[i].arrival_time);
            insertAtEnd(&gantt, processes[i].arrival_time, processes[i].arrival_time + processes[i].burst_time);
        }
        else {
            insertAtEnd(&gantt, prev_turnaround + idle_time, prev_turnaround + processes[i].burst_time + idle_time);
        }
    }

    printList(gantt);
    freeList(gantt);
}


int main(){
    struct Process processes[] = {
        {1, 3, 6},
        {2, 2, 4},
        {3, 4, 3},
        {4, 6, 5}
    };
    int n = sizeof(processes) / sizeof(processes[0]);

    fcfs(processes, n);
}