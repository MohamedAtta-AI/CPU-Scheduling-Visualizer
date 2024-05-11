#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define fcfs 0

#define N_ALGORITHMS 2
#define QUANTUM 10
#define EPOCHS 5

#define CPU_BURST_LOWER 5
#define CPU_BURST_UPPER 15

#define IO_BURST_LOWER 0
#define IO_BURST_UPPER 20

#define MAX_ARRIVAL 1000
#define MAX_NUM_PROC 100

#define FALSE 0
#define TRUE 1

typedef struct PCB {
    int pid,
        priority,
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

processPtr cloneJobQueue[MAX_NUM_PROC];
int cur_nproc_clone_JQ = 0;

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


void clone_JQ() {
    for (int i = 0; i < MAX_NUM_PROC; i++) 
        cloneJobQueue[i] = NULL;

    for (int i = 0; i < cur_nproc_JQ; i++) {
        processPtr cloneProcess = (processPtr)malloc(sizeof(PCB));
        cloneProcess->pid = jobQueue[i]->pid;
        cloneProcess->priority = jobQueue[i]->priority;
        cloneProcess->arrival = jobQueue[i]->arrival;
        cloneProcess->cpu_burst = jobQueue[i]->cpu_burst;
        cloneProcess->io_burst = jobQueue[i]->io_burst;
        cloneProcess->cpu_remaining = jobQueue[i]->cpu_remaining;
        cloneProcess->io_remaining = jobQueue[i]->io_remaining;
        cloneProcess->waiting = jobQueue[i]->waiting;
        cloneProcess->turnaround = jobQueue[i]->turnaround;
        cloneProcess->response = jobQueue[i]->response;
        cloneJobQueue[i] = cloneProcess;
    }
    cur_nproc_clone_JQ = cur_nproc_JQ;
}

void clear_JQ() {
    cur_nproc_JQ = 0;
    for (int i = 0; i < MAX_NUM_PROC; i++) {
        free(jobQueue[i]);
        jobQueue[i] = NULL;
    }
}

void loadClone_JQ() { // load clone to job queue
    clear_JQ();
    for (int i = 0; i < cur_nproc_clone_JQ; i++) {
        processPtr cloneProcess = (processPtr)malloc(sizeof(PCB));
        cloneProcess->pid = jobQueue[i]->pid;
        cloneProcess->priority = jobQueue[i]->priority;
        cloneProcess->arrival = jobQueue[i]->arrival;
        cloneProcess->cpu_burst = jobQueue[i]->cpu_burst;
        cloneProcess->io_burst = jobQueue[i]->io_burst;
        cloneProcess->cpu_remaining = jobQueue[i]->cpu_remaining;
        cloneProcess->io_remaining = jobQueue[i]->io_remaining;
        cloneProcess->waiting = jobQueue[i]->waiting;
        cloneProcess->turnaround = jobQueue[i]->turnaround;
        cloneProcess->response = jobQueue[i]->response;
        cloneJobQueue[i] = cloneProcess;
    }
    cur_nproc_clone_JQ = cur_nproc_JQ;
}

void clearClone_JQ() {
    for (int i = 0; i < MAX_NUM_PROC; i++) {
        free(cloneJobQueue[i]);
        cloneJobQueue[i] = NULL;
    }
}


int getProcByID_JQ(int pid) {
    // Return idx of process with the specified PID in ready queue
    for (int i = 0; i < cur_nproc_JQ; i++) {
        if (jobQueue[i]->pid == pid) 
            return i;
    }
    return -1;
}

int getProcByID_RQ(int pid) {
    for (int i = 0; i < cur_nproc_RQ; i++) {
        if (readyQueue[i]->pid == pid) 
            return i;
    }
    return -1;
}

int getProcByID_WQ(int pid) {
    for (int i = 0; i < cur_nproc_WQ; i++) {
        if (waitingQueue[i]->pid == pid) 
            return i;
    }
    return -1;
}


void insertInto_JQ(processPtr p) {
    if (cur_nproc_JQ < MAX_NUM_PROC) {
        int temp = getProcByID_JQ(p->pid);
        if (temp != -1) {
            printf("ERROR: Process with PID: %d already exists in job queue\n", p->pid);
            return;
        }
        jobQueue[cur_nproc_JQ++] = p;
    }
    else {
        puts("ERROR: Job queue is full");
        return;
    }
}

void insertInto_RQ(processPtr p) {
    if (cur_nproc_RQ < MAX_NUM_PROC) {
        int temp = getProcByID_RQ(p->pid);
        if (temp != -1) {
            printf("ERROR: Process with PID: %d already exists in ready queue\n", p->pid);
            return;
        }
        readyQueue[cur_nproc_RQ++] = p;
    }
    else {
        puts("ERROR: Ready queue is full");
        return;
    }
}

void insertInto_WQ(processPtr p) {
    if (cur_nproc_WQ < MAX_NUM_PROC) {
        int temp = getProcByID_WQ(p->pid);
        if (temp != -1) {
            printf("ERROR: Process with PID: %d already exists in waiting queue\n", p->pid);
            return;
        }
        waitingQueue[cur_nproc_WQ++] = p;
    }
    else {
        puts("ERROR: Waiting queue is full");
        return;
    }
}

void insertInto_T(processPtr p) {
    if (cur_nproc_T < MAX_NUM_PROC) {
        terminated[cur_nproc_T++] = p;
    }
    else {
        puts("ERROR: Can't terminate process");
        return;
    }
}


processPtr removeFrom_JQ(processPtr p) {
    if (cur_nproc_JQ > 0) {
        int temp = getProcByID_JQ(p->pid);
        if (temp == -1) {
            printf("ERROR: Can't find process with PID: %d\n", p->pid);
            return NULL;
        }
        else {
            processPtr removed = jobQueue[temp];

            for (int i = temp; i < cur_nproc_JQ - 1; i++)
                jobQueue[i] = jobQueue[i+1];
            jobQueue[cur_nproc_JQ - 1] = NULL;

            cur_nproc_JQ--;
            return removed;
        }
    }
}

processPtr removeFrom_RQ(processPtr p) {
    if (cur_nproc_RQ > 0) {
        int temp = getProcByID_RQ(p->pid);
        if (temp == -1) {
            printf("ERROR: Can't find process with PID: %d\n", p->pid);
            return NULL;
        }
        else {
            processPtr removed = readyQueue[temp];

            for (int i = temp; i < cur_nproc_RQ - 1; i++)
                readyQueue[i] = readyQueue[i+1];
            readyQueue[cur_nproc_RQ - 1] = NULL;

            cur_nproc_RQ--;
            return removed;
        }
    }
}

processPtr removeFrom_WQ(processPtr p) {
    if (cur_nproc_WQ > 0) {
        int temp = getProcByID_WQ(p->pid);
        if (temp == -1) {
            printf("ERROR: Can't find process with PID: %d\n", p->pid);
            return NULL;
        }
        else {
            processPtr removed = waitingQueue[temp];

            for (int i = temp; i < cur_nproc_WQ - 1; i++)
                waitingQueue[i] = waitingQueue[i+1];
            waitingQueue[cur_nproc_WQ - 1] = NULL;

            cur_nproc_WQ--;
            return removed;
        }
    }
}


void evaluate() {
    puts("\n\t{ Evaluation }\t\n");
    for (int i = 0; i < cur_eval_num; i++) {
        puts ("===========================================================");
        int algorithm = evals[i]->algorithm;
        int preemptive = evals[i]->preemptive;

        switch (algorithm)
        {
        case fcfs:
            puts("< FCFS Algorithm >");
            break;
        
        default:
            return;
        }

        puts ("-----------------------------------------------------------");
        printf("start time: %d / end time: %d / CPU utilization : %.2lf%% \n",evals[i]->start_time,evals[i]->end_time,evals[i]->cpu_utilization);
		printf("Average waiting time: %d\n",evals[i]->avg_waiting);
		printf("Average turnaround time: %d\n",evals[i]->avg_turnaround);
		printf("Average response time: %d\n",evals[i]->avg_response);
		printf("Completed: %d\n",evals[i]->completed);
    }
    puts ("===========================================================");
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


void analyze(int algorithm, int preemptive) {
    int total_wait, total_turnaround, total_response;
    total_wait = total_turnaround = total_response = 0;
    processPtr p = NULL;
    puts("===========================================================");
    for (int i =0; i < cur_nproc_T; i++) {
        p = terminated[i];
        printf("(PID: %d)\n", p->pid);
        printf("waiting time = %d, ", p->waiting);
        printf("turnaround time = %d, ", p->turnaround);
        printf("response time = %d\n", p->response);
        puts("===========================================================");
        total_wait += p->waiting;
        total_turnaround += p->turnaround;
        total_response += p->response;
    }
    printf("start time: %d / end time: %d / CPU utilization : %.2lf%% \n", 
        start, end, (double)(end - idle)/(end - start)*100);

    if (cur_nproc_T != 0) {
        printf("Average waiting time: %d\n", total_wait / cur_nproc_T);
		printf("Average turnaround time: %d\n", total_turnaround / cur_nproc_T);
		printf("Average response time: %d\n", total_response / cur_nproc_T);

        resultsPtr newRes = (resultsPtr)malloc(sizeof(results));
        newRes->algorithm = algorithm;
        newRes->preemptive = preemptive;
        newRes->start_time = start;
        newRes->end_time  = end;
        newRes->avg_waiting = total_wait / cur_nproc_T;
        newRes->avg_turnaround = total_turnaround / cur_nproc_T;
        newRes->avg_response = total_response / cur_nproc_T;
        newRes->cpu_utilization = (double)(end - idle) / (end - start)*100;
        newRes->completed = cur_nproc_T;
        evals[cur_eval_num++] = newRes;
    }
    printf("Completed: %d\n", cur_nproc_T);
    puts("===========================================================");
}


processPtr schedule(int algorithm, int preemptive, int quantum) {
    processPtr selectedProcess = NULL;

    switch (algorithm)
    {
    case fcfs:
        selectedProcess = FCFS();
        break;
    
    default:
        return NULL;
    }
    return selectedProcess;
}


void simulate(int time, int algorithm, int preemptive, int quantum) {
    processPtr arrived_process = NULL;
    int njobs = cur_nproc_JQ;

    // Load arrived jobs into ready queue
    for (int i = 0; i < cur_nproc_JQ; i++) {
        if (jobQueue[i]->arrival == time) {
            arrived_process = removeFrom_JQ(jobQueue[i--]);
            insertInto_RQ(arrived_process);
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
                insertInto_RQ(removeFrom_WQ(waitingQueue[i--]));
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
            insertInto_T(runningProcess);
            runningProcess = NULL;
        }
        else {
            // If an I/O operation needs to be performed, send the process to the waiting queue
            if (runningProcess->io_remaining > 0) {
                insertInto_WQ(runningProcess);
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
    insertInto_JQ(newProc);
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
   clone_JQ();
    // printJobQueue();
}


void main(int argc, char** argv) {
    initQueues();
    initEvals();

    // int total_nprocs = atoi(argv[1]);
    int total_nprocs = 10;
    initSimulation(total_nprocs);

    // int epochs = atoi(argv[2]);
    int epochs = 30;
    startSimulation(fcfs, FALSE, QUANTUM, epochs);
    evaluate();

    clearQueues();
    clearEvals();
}