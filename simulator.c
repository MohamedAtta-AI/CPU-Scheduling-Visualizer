#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_TIME_UNIT INT_MAX
#define MAX_PROCESS_NUM 100
#define MAX_RUNS 10
#define MAX_ALGORITHM_NUM 3

#define CPU_BURST_LOWER 5
#define CPU_BURST_UPPER 15

#define IO_BURST_LOWER 0
#define IO_BURST_UPPER 20

#define FCFS 0
#define LIF 1

#define TIME_QUANTUM 3

#define TRUE 1
#define FALSE 0


typedef struct PCB* processPointer;
typedef struct PCB {
    int pid;
    int priority;
    int arrivalTime;
    int CPUburst;
    int IOburst;
    int CPUremainingTime;
    int IOremainingTime;
    int waitingTime;
    int turnaroundTime;
    int responseTime;

}PCB;

int Computation_start = 0;
int Computation_end = 0;
int Computation_idle = 0;

typedef struct evaluation* evalPointer;
typedef struct evaluation {
	int alg;
	int preemptive;
	int startTime;
	int endTime;
	int avg_waitingTime;
	int avg_turnaroundTime;
	int avg_responseTime;
	double CPU_util;
	int completed;
}evaluation;

evalPointer evals[MAX_ALGORITHM_NUM];
int cur_eval_num = 0;

void init_evals() {
	cur_eval_num = 0;
	for(int i = 0; i < MAX_ALGORITHM_NUM; i++)
		evals[i] = NULL;
}

void clear_evals() {
	for(int i = 0; i < MAX_ALGORITHM_NUM; i++){
		free(evals[i]);
		evals[i]=NULL;
	}
	cur_eval_num = 0;
}


processPointer jobQueue[MAX_PROCESS_NUM];
int cur_proc_num_JQ = 0;

void init_JQ () {
	cur_proc_num_JQ = 0;
    int i;
    for (i = 0; i < MAX_PROCESS_NUM; i++)
        jobQueue[i] = NULL;
}

void sort_JQ() { // sort processes arriving at same time using pid
    int i, j;
    processPointer remember;
    for ( i = 1; i < cur_proc_num_JQ; i++ )
    {
      remember = jobQueue[(j=i)];
      while ( --j >= 0 && remember->pid < jobQueue[j]->pid )
          jobQueue[j+1] = jobQueue[j];
      jobQueue[j+1] = remember; 
    }
}

int getProcByPid_JQ (int givenPid) {
    // Return idx of process with the specified PID in ready queue
    int result = -1;
    int i;
    for(i = 0; i < cur_proc_num_JQ; i++) {
        int temp = jobQueue[i]->pid;
        if(temp == givenPid)
            return i;
    }
    return result;
}

void insertInto_JQ (processPointer proc) {
    if(cur_proc_num_JQ<MAX_PROCESS_NUM) {
        int temp = getProcByPid_JQ(proc->pid);
        if (temp != -1) {
            printf("<ERROR> The process with pid: %d already exists in Job Queue\n", proc->pid);
            return;  
        }
        jobQueue[cur_proc_num_JQ++] = proc;
    }
    else {
        puts("<ERROR> Job Queue is full");
        return;
    }
}

processPointer removeFrom_JQ (processPointer proc) { 
    if(cur_proc_num_JQ>0) {
        int temp = getProcByPid_JQ(proc->pid);
        if (temp == -1) {
            printf("<ERROR> Cannot find the process with pid: %d\n", proc->pid);
            return NULL;    
        } else {
            processPointer removed = jobQueue[temp];
            
            int i;
            for(i = temp; i < cur_proc_num_JQ - 1; i++)
                jobQueue[i] = jobQueue[i+1];   
            jobQueue[cur_proc_num_JQ - 1] = NULL;
            
            cur_proc_num_JQ--;
            return removed;
        }
        
    } else {
        puts("<ERROR> Job Queue is empty");
        return NULL;
    }
}

void clear_JQ() {
    int i;
    for(i = 0; i < MAX_PROCESS_NUM; i++) {
        free(jobQueue[i]);
        jobQueue[i] = NULL;
    }
    cur_proc_num_JQ = 0;
}

void print_JQ() {
	printf("Total number of processes: %d\n", cur_proc_num_JQ);
	int i;
	puts("pid    priority    arrival_time    CPU burst    IO burst");
	puts("========================================================");
    for(i = 0; i < cur_proc_num_JQ; i++) {
        printf("%3d    %8d    %12d    %9d    %8d\n", jobQueue[i]->pid, jobQueue[i]->priority, jobQueue[i]->arrivalTime, jobQueue[i]->CPUburst, jobQueue[i]->IOburst);   
    }
    puts("========================================================\n");
}

processPointer cloneJobQueue[MAX_PROCESS_NUM];
int cur_proc_num_clone_JQ = 0;

void clone_JQ() {
	// To leave the original JQ unchanged for other runs
	int i;
	for (i=0; i< MAX_PROCESS_NUM; i++) {
		cloneJobQueue[i] = NULL;
	}
	
	for (i=0; i<cur_proc_num_JQ; i++) {
		
		processPointer newProcess = (processPointer)malloc(sizeof(struct PCB));
        newProcess->pid = jobQueue[i]->pid;
        newProcess->priority = jobQueue[i]->priority;
        newProcess->arrivalTime = jobQueue[i]->arrivalTime;
        newProcess->CPUburst = jobQueue[i]->CPUburst;
        newProcess->IOburst = jobQueue[i]->IOburst;
        newProcess->CPUremainingTime = jobQueue[i]->CPUremainingTime;
        newProcess->IOremainingTime = jobQueue[i]->IOremainingTime;
        newProcess->waitingTime = jobQueue[i]->waitingTime;
        newProcess->turnaroundTime = jobQueue[i]->turnaroundTime;
        newProcess->responseTime = jobQueue[i]->responseTime;
        
        cloneJobQueue[i] = newProcess;
	}
	
	cur_proc_num_clone_JQ = cur_proc_num_JQ;
}

void loadClone_JQ() {
	 // load clone to job queue
	clear_JQ();
	int i;
	for (i=0; i<cur_proc_num_clone_JQ; i++) {
	
		processPointer newProcess = (processPointer)malloc(sizeof(struct PCB));
	    newProcess->pid = cloneJobQueue[i]->pid;
	    newProcess->priority = cloneJobQueue[i]->priority;
	    newProcess->arrivalTime = cloneJobQueue[i]->arrivalTime;
	    newProcess->CPUburst = cloneJobQueue[i]->CPUburst;
	    newProcess->IOburst = cloneJobQueue[i]->IOburst;
	    newProcess->CPUremainingTime = cloneJobQueue[i]->CPUremainingTime;
	    newProcess->IOremainingTime = cloneJobQueue[i]->IOremainingTime;
	    newProcess->waitingTime = cloneJobQueue[i]->waitingTime;
	    newProcess->turnaroundTime = cloneJobQueue[i]->turnaroundTime;
	    newProcess->responseTime = cloneJobQueue[i]->responseTime;
	    
	    jobQueue[i] = newProcess;
	}
	
	cur_proc_num_JQ = cur_proc_num_clone_JQ;
}

void clearClone_JQ() {
    int i;
    for(i = 0; i < MAX_PROCESS_NUM; i++) {
        free(cloneJobQueue[i]);
        cloneJobQueue[i] = NULL;
    }
}


processPointer runningProcess = NULL;
int timeConsumed = 0;

processPointer readyQueue[MAX_PROCESS_NUM];
int cur_proc_num_RQ = 0;

void init_RQ () {
    cur_proc_num_RQ = 0;
	int i;
    for (i = 0; i < MAX_PROCESS_NUM; i++)
        readyQueue[i] = NULL;
}

int getProcByPid_RQ (int givenPid) {
    // Return idx of process with the specified PID in ready queue
    int result = -1;
    int i;
    for(i = 0; i < cur_proc_num_RQ; i++) {
        int temp = readyQueue[i]->pid;
        if(temp == givenPid)
            return i;
    }
    return result;
}

void insertInto_RQ (processPointer proc) {
    if(cur_proc_num_RQ<MAX_PROCESS_NUM) {
        int temp = getProcByPid_RQ(proc->pid);
        if (temp != -1) {
            printf("<ERROR> The process with pid: %d already exists in Ready Queue\n", proc->pid);
            return;  
        }
        readyQueue[cur_proc_num_RQ++] = proc;
    }
    else {
        puts("<ERROR> Ready Queue is full");
        return;
    }
}

processPointer removeFrom_RQ (processPointer proc) {
    if(cur_proc_num_RQ>0) {
        int temp = getProcByPid_RQ(proc->pid);
        if (temp == -1) {
            printf("<ERROR> Cannot find the process with pid: %d\n", proc->pid);
            return NULL;    
        } else {
            processPointer removed = readyQueue[temp];
            
            int i;
            for(i = temp; i < cur_proc_num_RQ - 1; i++)
                readyQueue[i] = readyQueue[i+1];   
            readyQueue[cur_proc_num_RQ - 1] = NULL;
            
            cur_proc_num_RQ--;
            return removed;
        }
        
    } else {
        puts("<ERROR> Ready Queue is empty");
        return NULL;
    }
}

void clear_RQ() {
    int i;
    for(i = 0; i < MAX_PROCESS_NUM; i++) {
        free(readyQueue[i]);
        readyQueue[i]=NULL;
    }
    cur_proc_num_RQ = 0;
}

void print_RQ() {
	int i;
    for(i = 0; i < cur_proc_num_RQ; i++) {
        printf("%d ", readyQueue[i]->pid);   
    }
    printf("\ntotal number of processes: %d\n", cur_proc_num_RQ);
}


processPointer waitingQueue[MAX_PROCESS_NUM];
int cur_proc_num_WQ = 0; 

void init_WQ () {
	cur_proc_num_WQ = 0;
    int i;
    for (i = 0; i < MAX_PROCESS_NUM; i++)
        waitingQueue[i] = NULL;
}

int getProcByPid_WQ (int givenPid) {
    int result = -1;
    int i;
    for(i = 0; i < cur_proc_num_WQ; i++) {
        int temp = waitingQueue[i]->pid;
        if(temp == givenPid)
            return i;
    }
    return result;
}

void insertInto_WQ (processPointer proc) {
    if(cur_proc_num_WQ<MAX_PROCESS_NUM) {
        int temp = getProcByPid_WQ(proc->pid);
        if (temp != -1) {
            printf("<ERROR> The process with pid: %d already exists in Waiting Queue\n", proc->pid);
            return;  
        }
        waitingQueue[cur_proc_num_WQ++] = proc;
    }
    else {
        puts("<ERROR> Waiting Queue is full");
        return;
    }
}

processPointer removeFrom_WQ (processPointer proc) { 
    if(cur_proc_num_WQ>0) {
        int temp = getProcByPid_WQ(proc->pid);
        if (temp == -1) {
            printf("<ERROR> Cannot find the process with pid: %d\n", proc->pid);
            return NULL;    
        } else {
        	
            processPointer removed = waitingQueue[temp];
            int i;
            for(i = temp; i < cur_proc_num_WQ - 1; i++)
                waitingQueue[i] = waitingQueue[i+1];
				   
            waitingQueue[cur_proc_num_WQ - 1] = NULL;
            
            cur_proc_num_WQ--;
            
            return removed;
        }
        
    } else {
        puts("<ERROR> Waiting Queue is empty");
        return NULL;
    }
}

void clear_WQ() {
    int i;
    for(i = 0; i < MAX_PROCESS_NUM; i++) {
        free(waitingQueue[i]);
        waitingQueue[i] = NULL;
    }
    cur_proc_num_WQ = 0;
}

void print_WQ() {
    puts("\nprintf_WQ()");
	int i;
    
    for(i = 0; i < cur_proc_num_WQ; i++) {
        printf("%d ", waitingQueue[i]->pid);   
    }
    printf("\nTotal number of processes: %d\n", cur_proc_num_WQ);
}


processPointer terminated[MAX_PROCESS_NUM];
int cur_proc_num_T = 0; 

void init_T () {
	cur_proc_num_T = 0;
    int i;
    for (i = 0; i < MAX_PROCESS_NUM; i++)
        terminated[i] = NULL;
}

void clear_T() { 
    int i;
    for(i = 0; i < MAX_PROCESS_NUM; i++) {
        free(terminated[i]);
        terminated[i] = NULL;
    }
    cur_proc_num_T = 0;
}

void insertInto_T (processPointer proc) {
    if(cur_proc_num_T<MAX_PROCESS_NUM) {
        terminated[cur_proc_num_T++] = proc;
    }
    else {
        puts("<ERROR> Cannot terminate the process");
        return;
    }
}

void print_T() { 
	int i;
    for(i = 0; i < cur_proc_num_T; i++) {
        printf("%d ", terminated[i]->pid);   
    }
    printf("\nTotal number of processes: %d\n", cur_proc_num_T);
}

processPointer createProcess(int pid, int priority, int arrivalTime, int CPUburst, int IOburst) {
    if (arrivalTime > MAX_TIME_UNIT || arrivalTime < 0) {
        printf("<ERROR> arrivalTime should be in [0..MAX_TIME_UNIT]\n");
        printf("<USAGE> createProcess(int pid, int priority, int arrivalTime, int CPUburst, int IOburst)\n");
        return NULL;
    }
    
    if (CPUburst <= 0 || IOburst < 0) {
        printf("<ERROR> CPUburst and should be larger than 0 and IOburst cannot be a negative number.\n");
        printf("<USAGE> createProcess(int pid, int priority, int arrivalTime, int CPUburst, int IOburst)\n");
        return NULL;
    }
        processPointer newProcess = (processPointer)malloc(sizeof(struct PCB));
        newProcess->pid = pid;
        newProcess->priority = priority;
        newProcess->arrivalTime = arrivalTime;
        newProcess->CPUburst = CPUburst;
        newProcess->IOburst = IOburst;
        newProcess->CPUremainingTime = CPUburst;
        newProcess->IOremainingTime = IOburst;
        newProcess->waitingTime = 0;
        newProcess->turnaroundTime = 0;
        newProcess->responseTime = -1;

        insertInto_JQ(newProcess);

    //printf("%d %d %d %d created\n",newProcess.pid ,newProcess.priority, newProcess.arrivalTime, newProcess.CPUburst);
    return newProcess;
}


processPointer FCFS_alg() {
    processPointer earliestProc = readyQueue[0];
    
    if (earliestProc != NULL && runningProcess == NULL){
            return removeFrom_RQ(earliestProc);
    } 
    else { 
        return runningProcess;
    }
}

processPointer LIF_alg(int preemptive) {
	processPointer longestJob = readyQueue[0]; 
	if(longestJob != NULL) {
        for(int i = 0; i < cur_proc_num_RQ; i++) {
            if (readyQueue[i]->IOremainingTime >= longestJob->IOremainingTime) { 
                // If remaining times are equal, the process that arrived first is executed
                if(readyQueue[i]->IOremainingTime == longestJob->IOremainingTime) {
                    if (readyQueue[i]->arrivalTime < longestJob->arrivalTime) {
                        longestJob = readyQueue[i];
                    }
                } 
                else {
                    longestJob = readyQueue[i];
                }
            }
        }
		
		if(runningProcess != NULL) {
            if(preemptive){
            
                if(runningProcess->IOremainingTime <= longestJob->IOremainingTime) {
                    // If remaining times are equal, the process that arrived first is executed
                    if(runningProcess->IOremainingTime == longestJob->IOremainingTime) { 
                        if (runningProcess->arrivalTime < longestJob->arrivalTime){
                            return runningProcess;
                        } 
                        // If the arrival times are also equal, preempting is avoided to reduce context-switch overhead.
                        else if(runningProcess->arrivalTime == longestJob->arrivalTime) {
                            return runningProcess;  
                        }
                    }
                    puts("preemption is detected.");
                    insertInto_RQ(runningProcess);
                    return removeFrom_RQ(longestJob);
                }
            }
            return runningProcess;
        } 
        else {
            return removeFrom_RQ(longestJob);
        }
	}
    else {
		return runningProcess;
	}
}


processPointer schedule(int alg, int preemptive, int time_quantum) {
	processPointer selectedProcess = NULL;
    
    switch(alg) {
        case FCFS:
            selectedProcess = FCFS_alg();
            break;
        case LIF:
        	selectedProcess = LIF_alg(preemptive);
        	break;
        default:
            return NULL;
    }
    
    return selectedProcess;
}

void simulate(int amount, int alg, int preemptive, int time_quantum) {
	processPointer tempProcess = NULL;
	int jobNum = cur_proc_num_JQ;

	// Load arrived jobs into ready queue
	int i;
	for(i = 0; i < cur_proc_num_JQ; i++) {
		if(jobQueue[i]->arrivalTime == amount) {
			tempProcess = removeFrom_JQ(jobQueue[i--]);
			insertInto_RQ(tempProcess);
		}
	}
	processPointer prevProcess = runningProcess;
	runningProcess = schedule(alg, preemptive, time_quantum);
	
	printf("%d: ",amount);
	if(prevProcess != runningProcess) { // if running process changed
		timeConsumed = 0; // reset time consumed for running process
		
		if(runningProcess->responseTime == -1) { // response time not recorded yet
			runningProcess->responseTime = amount - runningProcess->arrivalTime;
		}
	}
	
    // Make processes in ready queue wait
    for(i = 0; i < cur_proc_num_RQ; i++) { 
        
        if(readyQueue[i]) {
        	readyQueue[i]->waitingTime++;
        	readyQueue[i]->turnaroundTime++;
    	}
    }
	
    // Processes in waiting perform IO operations
    for(i = 0; i < cur_proc_num_WQ; i++) { 
		if(waitingQueue[i]) {
			waitingQueue[i]->turnaroundTime++;
			waitingQueue[i]->IOremainingTime--;
			
            // When the I/O operation is completed, add to ready queue
			if(waitingQueue[i]->IOremainingTime <= 0 ) { 
				printf("(pid: %d) -> IO complete, ", waitingQueue[i]->pid); 
				insertInto_RQ(removeFrom_WQ(waitingQueue[i--]));
			}
		}
	}
	
    // Execute currently running process
    if(runningProcess != NULL) {
        runningProcess->CPUremainingTime --;
        runningProcess->turnaroundTime ++;
        timeConsumed ++;
        printf("(pid: %d) -> running ",runningProcess->pid);
        
        // If all processes have completed their execution, send them to the "terminated" state
        if(runningProcess->CPUremainingTime <= 0) {
			insertInto_T(runningProcess);
			runningProcess = NULL;
			printf("-> terminated");
		} 
        else {
            // If an I/O operation needs to be performed, send the process to the waiting queue
			if(runningProcess->IOremainingTime > 0) {
				insertInto_WQ(runningProcess);
				runningProcess = NULL;
				printf("-> IO request");	
			}
		}
		
        printf("\n");
    } else { 
    	printf("idle\n");
    	Computation_idle++;
	}
}

void analyze(int alg, int preemptive) {
	int wait_sum = 0;
	int turnaround_sum = 0;
	int response_sum = 0;
	int i;
	processPointer p=NULL;
	puts  ("===========================================================");
	for(i = 0; i < cur_proc_num_T; i++){
		p = terminated[i];
		printf("(pid: %d)\n",p->pid);
		printf("waiting time = %d, ",p->waitingTime);
		printf("turnaround time = %d, ",p->turnaroundTime);
		printf("response time = %d\n",p->responseTime);
		
		puts  ("===========================================================");
		wait_sum += p->waitingTime;
		turnaround_sum += p->turnaroundTime;
		response_sum += p->responseTime;
	}
	// printf("start time: %d / end time: %d / CPU utilization : %.2lf%% \n",Computation_start, Computation_end,
	//  (double)(Computation_end - Computation_idle)/(Computation_end - Computation_start)*100);
	
    evalPointer newEval = (evalPointer)malloc(sizeof(struct evaluation));
	if(cur_proc_num_T != 0) {
		newEval->alg = alg;
		newEval->preemptive = preemptive;
		
		newEval->startTime = Computation_start;
		newEval->endTime = Computation_end;
		newEval->avg_waitingTime = wait_sum / cur_proc_num_T;
		newEval->avg_turnaroundTime = turnaround_sum / cur_proc_num_T;
		newEval->avg_responseTime = response_sum / cur_proc_num_T;
		newEval->CPU_util = (double)(Computation_end - Computation_idle) / (Computation_end - Computation_start)*100;
		newEval->completed = cur_proc_num_T;
		evals[cur_eval_num++] = newEval;

		// printf("Average waiting time: %d\n", newEval->avg_waitingTime);
		// printf("Average turnaround time: %d\n", newEval->avg_turnaroundTime);
		// printf("Average response time: %d\n", newEval->avg_responseTime);
	}	
	// printf("Completed: %d\n", cur_proc_num_T);
	// puts  ("===========================================================");
}

void startSimulation(int alg, int preemptive, int time_quantum, int total_time) {
	loadClone_JQ();
	
	switch(alg) {
        case FCFS:
            puts("<FCFS Algorithm>");
            break;
        case LIF:
        	if(preemptive) printf("<Preemptive ");
        	else printf("<Non-preemptive ");
        	puts("LIF Algorithm>");
        	break;
        default:
            return;
    }
	
	int initial_proc_num = cur_proc_num_JQ; 
	
	int i;
	if(cur_proc_num_JQ <= 0) {
		puts("<ERROR> Simulation failed. Process doesn't exist in the job queue");
		return;
	}
	
    // find first arrived process to start with
	int minArriv = jobQueue[0]->arrivalTime;
	for(i = 0; i < cur_proc_num_JQ; i++) {
		if(minArriv > jobQueue[i]->arrivalTime)
			minArriv = jobQueue[i]->arrivalTime;		
	}
	Computation_start = minArriv;
	Computation_idle = 0;
	for(i = 0; i < total_time; i++) {
		simulate(i, alg, preemptive, TIME_QUANTUM);
		if(cur_proc_num_T == initial_proc_num) {
			i++;
			break;
		}
	}
	Computation_end = i-1;
	
	analyze(alg, preemptive);
	clear_JQ();
    clear_RQ();
    clear_T();
    clear_WQ();
    free(runningProcess);
    runningProcess = NULL;
    timeConsumed = 0;
    Computation_start = 0;
	Computation_end = 0;
	Computation_idle = 0;
}

void evaluate() {
	puts ("\n                       <Evaluation>                    \n");
	for(int i = 0; i < cur_eval_num; i++) {
		
		puts ("===========================================================");
		int alg = evals[i]->alg;
		int preemptive = evals[i]->preemptive;
		
		switch (evals[i]->alg) {
            case FCFS:
                puts("<FCFS Algorithm>");
                break;
            case LIF:
                if(preemptive) printf("<Preemptive ");
                else printf("<Non-preemptive ");
                puts("LIF Algorithm>");
                break;
            default:
                return;
		}
		puts ("-----------------------------------------------------------");
		printf("start time: %d / end time: %d / CPU utilization : %.2lf%% \n",evals[i]->startTime,evals[i]->endTime,evals[i]->CPU_util);
		printf("Average waiting time: %d\n",evals[i]->avg_waitingTime);
		printf("Average turnaround time: %d\n",evals[i]->avg_turnaroundTime);
		printf("Average response time: %d\n",evals[i]->avg_responseTime);
		printf("Completed: %d\n",evals[i]->completed);
	}
	puts  ("===========================================================");
}


void createProcesses(int total_num, int io_num) {
	if(io_num > total_num) {
		puts("<ERROR> The number of IO event cannot be higher than the number of processes");
		exit(-1);
	}
		
	int i;
	for(i=0;i<total_num; i++) {
		int priority = rand() % total_num + 1;
        int arrival = rand() % (total_num + 10);
        int cpu_burst = rand() % (CPU_BURST_UPPER - CPU_BURST_LOWER + 1) + CPU_BURST_LOWER;
		createProcess(i+1, rand() % total_num + 1, rand() % (total_num + 10), rand() % 16 + 5, 0);
	}
	
	for(i=0;i<io_num;i++) {
		
		int randomIndex = rand() % total_num ;
		if(jobQueue[randomIndex]->IOburst ==0) {
			int randomIOburst = rand() % 10 + 1;
			jobQueue[randomIndex]->IOburst = randomIOburst;
			jobQueue[randomIndex]->IOremainingTime = randomIOburst;
		} 
        else {
			i--;
		}
		
	}
	sort_JQ();
	clone_JQ(); //backup this JQ
	print_JQ();
}

void main(int argc, char **argv) {
    init_RQ();
    init_JQ();
    init_T();
    init_WQ();
    init_evals();

	srand(time(NULL));

    int totalProcessNum = atoi(argv[1]);
    int totalIOProcessNum = atoi(argv[2]); 
    int runs = atoi(argv[3]);
    
    for (int i = 0; i < runs; i++) {
        puts  ("===========================================================");
        printf("                          RUN #%d\n", i+1);
        puts  ("===========================================================");

        createProcesses(totalProcessNum,totalIOProcessNum);

        startSimulation(FCFS,FALSE,TIME_QUANTUM, MAX_TIME_UNIT);
        startSimulation(LIF,FALSE, TIME_QUANTUM, MAX_TIME_UNIT);
        startSimulation(LIF,TRUE, TIME_QUANTUM, MAX_TIME_UNIT);
        evaluate();

        clear_RQ();
        clear_T();
        clear_WQ();
        clear_JQ();
        clearClone_JQ();
        clear_evals();
    }
}