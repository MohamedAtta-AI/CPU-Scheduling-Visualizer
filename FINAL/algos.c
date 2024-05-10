#include "sorting.c"

double results[3];

void printProcessTable(Process p[], int n)
{
    int i;

    puts("\t+-----+------------+-------------+----------+-------------+-----------------+--------------+-----------------+");
    puts("\t| PID | Burst Time | Arrive Time | Priority | Return Time |  Response Time  | Waiting Time | Turnaround Time |");
    puts("\t+-----+------------+-------------+----------+-------------+-----------------+--------------+-----------------+");

    for (i = 0; i < n; i++)
    {
        printf("\t| %3s |     %3d    |     %3d     |    %3d   |     %3d     |      %3d        |      %3d     |        %3d      |\n",
               p[i].id, p[i].burst, p[i].arrive_time, p[i].priority, p[i].return_time, p[i].response_time, p[i].waiting_time, p[i].turnaround_time);

        puts("\t+-----+------------+-------------+----------+-------------+-----------------+--------------+-----------------+");
    }

    puts("\n");
}

void printGanttChart(Process *procs, int numProcs)
{
    int i, j;

    printf("\t ");

    for (i = 0; i < numProcs; i++)
    {
        for (j = 0; j < procs[i].burst; j++)
            printf("--");

        printf(" ");
    }

    printf("\n\t|");

    for (i = 0; i < numProcs; i++)
    {
        for (j = 0; j < procs[i].burst - 1; j++)
            printf(" ");

        printf("%s", procs[i].id);

        for (j = 0; j < procs[i].burst - 1; j++)
            printf(" ");

        printf("|");
    }

    printf("\n\t ");

    for (i = 0; i < numProcs; i++)
    {
        for (j = 0; j < procs[i].burst; j++)
            printf("--");

        printf(" ");
    }

    printf("\n\t");

    printf("0");

    for (i = 0; i < numProcs; i++)
    {
        for (j = 0; j < procs[i].burst; j++)
            printf("  ");

        if (procs[i].return_time > 9)
            printf("\b");

        printf("%d", procs[i].return_time);
    }

    printf("\n");
}


double* FCFS(Process *procs, int numProcs)
{
    int i;
    int totalWaiting = 0;
    int totalTurnaround = 0;
    int totalResponse = 0;
    int totalReturn = 0;

    initializeProcesses(procs, numProcs);
    MergeSortByArrival(procs, 0, numProcs);

    procs[0].return_time = procs[0].burst;
    procs[0].turnaround_time = procs[0].return_time - procs[0].arrive_time;
    procs[0].response_time = 0;
    procs[0].waiting_time = 0;

    totalResponse += procs[0].response_time;
    totalWaiting += procs[0].waiting_time;
    totalTurnaround += procs[0].turnaround_time;
    totalReturn += procs[0].burst;

    for (i = 1; i < numProcs; i++)
    {
        procs[i].waiting_time = totalReturn - procs[i].arrive_time;
        procs[i].return_time = totalReturn + procs[i].burst;
        procs[i].turnaround_time = procs[i].return_time - procs[i].arrive_time;
        procs[i].response_time = procs[i].waiting_time;

        totalReturn += procs[i].burst;
        totalWaiting += procs[i].waiting_time;
        totalTurnaround += procs[i].turnaround_time;
        totalResponse += procs[i].response_time;
    }


    printf("\n\tFCFS Scheduling Algorithm\n\n");
    results[0] = (double)totalWaiting / (double)numProcs; // Avg waiting
    results[1] = (double)totalTurnaround / (double)numProcs; // Avg turnaround
    results[2] = (double)totalResponse / (double)numProcs; // Avg response
    return results;
    printGanttChart(procs, numProcs);

    printf("\n\tAverage Waiting Time     : %-2.2lf\n", (double)totalWaiting / (double)numProcs);
    printf("\tAverage Turnaround Time  : %-2.2lf\n", (double)totalTurnaround / (double)numProcs);
    printf("\tAverage Response Time    : %-2.2lf\n\n", (double)totalResponse / (double)numProcs);

    printProcessTable(procs, numProcs);
}


double* HRN(Process *p, int len)
{
    int time, idx;
    int total_burst_time = 0;
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    int total_response_time = 0;
    float hrr, temp;

    initializeProcesses(p, len);

    int i, j;
    for (i = 0; i < len; i++)
    {
        total_burst_time += p[i].burst;
    }

    MergeSortByArrival(p, 0, len);

    for (time = p[0].arrive_time; time < total_burst_time;)
    {
        hrr = -9999;

        for (i = 0; i < len; i++)
        {
            if ((p[i].arrive_time <= time) && (p[i].completed != 1))
            {
                temp = (p[i].burst + (time - p[i].arrive_time)) / p[i].burst;

                if (hrr < temp)
                {
                    hrr = temp;
                    idx = i;
                }
            }
        }

        time += p[idx].burst;

        p[idx].waiting_time = time - p[idx].arrive_time - p[idx].burst;
        p[idx].turnaround_time = time - p[idx].arrive_time;
        p[idx].return_time = p[idx].turnaround_time + p[idx].arrive_time;
        p[idx].response_time = p[idx].waiting_time;
        p[idx].completed = 1;

        total_waiting_time += p[idx].waiting_time;
        total_turnaround_time += p[idx].turnaround_time;
        total_response_time += p[idx].response_time;
    }

    QuickSortByReturn(p, len);

    printf("\tHighest Response Ratio Next Scheduling Algorithm\n\n");
    results[0] = (double)total_waiting_time / (double)len; // Avg waiting
    results[1] = (double)total_turnaround_time / (double)len; // Avg turnaround
    results[2] = (double)total_response_time / (double)len; // Avg response
    return results;
    printGanttChart(p, len);

    printf("\n\tAverage Waiting Time     : %-2.2lf\n", (double)total_waiting_time / (double)len);
    printf("\tAverage Turnaround Time  : %-2.2lf\n", (double)total_turnaround_time / (double)len);
    printf("\tAverage Response Time    : %-2.2lf\n\n", (double)total_response_time / (double)len);

    printProcessTable(p, len);
}



