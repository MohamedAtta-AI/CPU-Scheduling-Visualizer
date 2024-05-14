# CPU-Scheduling-Simulator
A terminal tool built with C for intuitive understanding and simulation of CPU scheduling algorithms in operating systems.

### Configurations
This simulation uses a random generator for the processes' attributes, the top of the file contains configurable macros:
```c
#define MAX_TIME_UNIT INT_MAX // Maximum number of time units for a simulation run
#define MAX_PROCESS_NUM 100   // Maximum number of input processes
#define MAX_RUNS 10           // Maximum number of input runs
#define MAX_ALGORITHM_NUM 3   // Set this to the number of implemented algorithms that you will use

// Bounds for the random generator
#define CPU_BURST_LOWER 5
#define CPU_BURST_UPPER 15

#define IO_BURST_LOWER 0
#define IO_BURST_UPPER 20

// For switch statements. You can add more algorithms
#define FCFS 0
#define LIF 1

// For Round Robin algorithm (not implemented here)
#define TIME_QUANTUM 3
```

### Getting Started:
- Download `simulator.c`
- Compile the file using the command:
`gcc -o simulator simulator.c`
- Execute the file on linux terminal with three arguments<br>
(1) The total number of processes<br>
(2) The number of processes that will call I/O request<br>
(3) The number of runs<br>
- Examples<br>
`./simulator 10 3 5`<br>
If you want to log the results in a file, use the output redirection operator:<br>
`./simulator 10 3 5 >> result.txt`
<br>
