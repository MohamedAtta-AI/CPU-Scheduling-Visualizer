#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define fcfs 0

#define CPU_BURST_LOWER 5
#define CPU_BURST_UPPER 15

#define IO_BURST_LOWER 0
#define IO_BURST_UPPER 20

#define MAX_ARRIVAL 1000
#define MAX_NUM_PROC 100

#define N_ALGORITHMS 2


typedef struct PCB {
    int pid,
        arrival,
        cpu_burst,
        io_burst,
        cpu_remaining,
        io_remaining,
        waiting,
        turnaround,
        response;
} PCB;
typedef PCB* processPtr;

int start = 0; int end = 0; int idle = 0;

processPtr jobQueue[MAX_NUM_PROC];
int cur_nproc_JQ = 0;

processPtr readyQueue[MAX_NUM_PROC];
int cur_nproc_RQ = 0;

processPtr waitingQueue[MAX_NUM_PROC];
int cur_nproc_WQ = 0;

processPtr terminated[MAX_NUM_PROC];
int cur_nproc_T = 0;

processPtr runningProcess = NULL;
int time_consumed = 0;


typedef struct results {
    int algorithm,
        preemptive,
        start_time,
        end_time,
        avg_waiting,
        avg_turnaround,
        avg_response,
        completed;
    double cpu_utilization;
} results;
typedef results* resultsPtr;

resultsPtr evals[N_ALGORITHMS];
int cur_eval_num = 0;


void initQueues() {
    cur_nproc_JQ = cur_nproc_RQ = cur_nproc_WQ = cur_nproc_T = 0;
    for (int i = 0; i < MAX_NUM_PROC; i++) {
        jobQueue[i] = NULL;
        readyQueue[i] = NULL;
        waitingQueue[i] = NULL;
        terminated[i] = NULL;
    }
}

void clearQueues() {
    cur_nproc_JQ = cur_nproc_RQ = cur_nproc_WQ = cur_nproc_T = 0;
    for (int i = 0; i < MAX_NUM_PROC; i++) {
        free(jobQueue[i]);
        jobQueue[i] = NULL;
        free(readyQueue[i]);
        readyQueue[i] = NULL;
        free(waitingQueue[i]);
        waitingQueue[i] = NULL;
        free(terminated[i]);
        terminated[i] = NULL;
    }
}

void initEvals() {
    cur_eval_num = 0;
    for (int i = 0; i < N_ALGORITHMS; i++) evals[i] = NULL;
}

void clearEvals() {
	cur_eval_num = 0;
    for(int i = 0; i < N_ALGORITHMS; i++){
		free(evals[i]);
		evals[i]=NULL;
	}
}


int getProcByID_JQ(int pid) {
    for (int i = 0; i < cur_nproc_JQ; i++) {
        if (jobQueue[i]->pid == pid) 
            return i;
    }
    return -1;
}


processPtr FCFS() {
    processPtr earliestProc = readyQueue[0];
    if (earliestProc != NULL && runningProcess == NULL) {
        return removeFrom_RQ(earliestProc);
    } 
    else {
        return runningProcess;
    }
}


void simulate(int time, int algorithm, int preemptive, int quantum) {
    processPtr arrived_process = NULL;
    int njobs = cur_nproc_JQ;

    // Load arrived jobs into ready queue
    for (int i = 0; i < cur_nproc_JQ; i++) {
        if (jobQueue[i]->arrival == time) {
            arrived_process = removeFromJobQueue(jobQueue[i--]);
            insertIntoReadyQueue(arrived_process);
        }
    }
    processPtr prevProcess = runningProcess;
    runningProcess = schedule(algorithm, preemptive, quantum);

    printf("%d: ", time);
    // if running process changed
    if (prevProcess != runningProcess) {
        time_consumed = 0; // reset time consumed for running process
        if (runningProcess->response == -1) { // response time not recorded yet
            runningProcess->response = time - runningProcess->arrival;
        }
    }

    // Make processes in ready queue wait
    for (int i = 0; i < cur_nproc_RQ; i++) {
        if (readyQueue[i]) {
            readyQueue[i]->waiting++;
            readyQueue[i]->turnaround++;
        }
    }

    // Processes in waiting perform IO operations
    for (int i = 0; i < cur_nproc_WQ; i++) {
        if (waitingQueue[i]) {
            waitingQueue[i]->turnaround++;
            waitingQueue[i]->io_remaining--;

            // When the I/O operation is completed, add to ready queue
            if (waitingQueue[i]->io_remaining <= 0) {
                insertIntoReadyQueue(removeFromWaitingQueue(waitingQueue[i--]));
            }
        }
    }

    // Execute currently running process
    if (runningProcess != NULL) {
        runningProcess->turnaround++;
        runningProcess->cpu_remaining--;
        time_consumed++;

        // If all processes have completed their execution, send them to the "terminated" state
        if (runningProcess->cpu_remaining <= 0) {
            insertIntoTerminated(runningProcess);
            runningProcess = NULL;
        }
        else {
            // If an I/O operation needs to be performed, send the process to the waiting queue
            if (runningProcess->io_remaining > 0) {
                insertIntoWaitingQueue(runningProcess);
                runningProcess = NULL;
            }
        }
    }
    else {
        idle++;
    }
}


void startSimulation(int algorithm, int preemptive, int quantum, int total_time) {
    loadClone_JQ();

    switch (algorithm) {
        case fcfs:
            puts("{\tFirst Come First Serve (FCFS)\t}");
            break;
        default:
            return;
    }

    int initial_nproc = cur_nproc_JQ;

    // find first arrived process to start with
    int min_arrival = jobQueue[0]->arrival;
    for (int i = 0; i < cur_nproc_JQ; i++)
        if (jobQueue[i]->arrival < min_arrival)
            min_arrival = jobQueue[i]->arrival;
    
    start = min_arrival;
    idle = 0;

    int i;
    for (i = 0; i < total_time; i++) {
        simulate(i, algorithm, preemptive, quantum);
        if (cur_nproc_T == initial_nproc) {
            ++i;
            break;
        }
    }
    end = i - 1;

    analyze(algorithm, preemptive);

    clearQueues();
    free(runningProcess);
    runningProcess = NULL;
    time_consumed = start = end = idle = 0;
}


processPtr createProcess(int pid, int priority, int arrival, int cpu_burst, int io_burst) {
    processPtr newProc = (processPtr)malloc(sizeof(PCB));
    newProc->pid = pid;
    newProc->arrival = arrival;
    newProc->cpu_burst = cpu_burst; 
    newProc->io_burst = io_burst;
    newProc->cpu_remaining = cpu_burst;
    newProc->io_remaining = io_burst;
    newProc->waiting = 0;
    newProc->turnaround = 0;
    newProc->response = -1;
    return newProc;
}


void initSimulation(int total_nprocs) {
    srand(time(NULL));

    for (int i = 0; i < total_nprocs; i++) {
        int priority = rand() % total_nprocs + 1;
        int arrival = rand() % (total_nprocs + 10);
        int cpu_burst = rand() % (CPU_BURST_UPPER - CPU_BURST_LOWER + 1) + CPU_BURST_LOWER;
        int io_burst = rand() % (IO_BURST_UPPER - IO_BURST_LOWER + 1) + IO_BURST_LOWER;
        createProcess(i+1, priority, arrival, cpu_burst, io_burst);
    }

    // sortJobQueue();
    // cloneJobQueue();
    printJobQueue();
}


void main(int argc, char** argv) {
    initQueues();
    initEvals();

    int total_nprocs = atoi(argv[1]);
    initSimulation(total_nprocs);

    int epochs = atoi(argv[2]);
    startSimulation(fcfs, );
}