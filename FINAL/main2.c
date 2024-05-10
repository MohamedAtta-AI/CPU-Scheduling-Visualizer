#include "algos.c"
#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 100
#define MAX_FILES 5
#define FILENAME_LENGTH 20


void printResults(double* results) {
    printf("\n\tAverage Waiting Time     : %-2.2lf\n", results[0]);
    printf("\tAverage Turnaround Time  : %-2.2lf\n", results[1]);
    printf("\tAverage Response Time    : %-2.2lf\n\n", results[2]);
}


void readProcessFile(char *filename, struct Process processes[], int *numProcesses) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }

    fscanf(file, "%d", numProcesses);

    for (int i = 0; i < *numProcesses; i++) {
        fscanf(file, "%s %d %d %d",
               processes[i].id, &processes[i].arrive_time, &processes[i].burst, &processes[i].priority);
    }

    fclose(file);
}

int main() {
    char filenames[MAX_FILES][FILENAME_LENGTH] = {
        "process_1.txt", "process_2.txt", "process_3.txt", "process_4.txt", "process_5.txt"};
    int numProcesses;
    Process *process;

    for (int fileIndex = 0; fileIndex < MAX_FILES; fileIndex++) {
        process = NULL;
        numProcesses = 0;

        readProcessFile(filenames[fileIndex], process, &numProcesses);

        process = (Process *)malloc(sizeof(Process) * numProcesses);

        if (process == NULL) {
            printf("Memory allocation error\n");
            exit(1);
        }

        // readProcessFile(filenames[fileIndex], process, &numProcesses);

        printf("\nProcessing file: %s\n", filenames[fileIndex]);

        double* fcfs = FCFS(process, numProcesses);
        printResults(fcfs);

        double* hrn = HRN(process, numProcesses);
        printResults(hrn);

        free(process);
    }

    system("pause");
    return 0;
}
