#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

#define MAX_PROCESSES 4 // maximum number of processes
#define MAX_PROCESS_TIME 15 // maximum time a process can take to finish
#define MAX_IO 3 // maximum number of IOs a process can have
#define QUANTUM 5

enum PROCESS_IO { DISK = 3, TAPE = 5, PRINTER = 8 };

enum PROCESS_STATE { READY, RUNNING, BLOCKED, FINISHED };

enum PROCESS_PRIORITY { HIGH, LOW };

typedef struct IO {
    char name[8];
    int startTime;
    int duration;
} IO;

// estruturas que precisaremos
typedef struct {
    int arrivalTime;
    int serviceTime;
    int processedTime;

    int pid;
    int ppid;
    int state;
    int priority;

    IO *ios;
    int currentIO;
    int numIOs;
    int elapsedIOTime;
} Process;

typedef struct QueueElement {
    Process *process;
    struct QueueElement *next;
} QueueElement;

typedef struct {
    QueueElement *first;
    QueueElement *last;
} Queue;

typedef struct {
    Queue *highPriority;
    Queue *lowPriority;
    Queue *disk;
    Queue *tape;
    Queue *printer;
} Queues;

typedef struct {
    Queues *queues;
    Process *processes;

    int TerminatedProcesses;
    int numProcesses;

    Process *currentCPUProcess;
    int quantum;
    int elapsedQuantum;
} Scheduler;

/*
    creates a new process with the given parameters
*/
Process newProcess(int pid, int arrivalTime, int serviceTime, int numIOs, IO *ios) {
    Process process;

    process.pid = pid;
    process.state = READY;
    process.priority = HIGH;
    process.ppid = -1;
    
    process.arrivalTime = arrivalTime;
    process.serviceTime = serviceTime;
    process.processedTime = 0;

    process.currentIO = 0;
    process.numIOs = numIOs;
    process.ios = ios;
    process.elapsedIOTime = 0;

    return process;
}

/*
    sorts the processes by arrival time
*/
void sortProcesses(Process *processes, int numProcesses) {
    Process aux;
    for (int i = 0; i < numProcesses; i++) {
        for (int j = i + 1; j < numProcesses; j++) {
            if (processes[i].arrivalTime > processes[j].arrivalTime) {
                aux = processes[i];
                processes[i] = processes[j];
                processes[j] = aux;
            }
        }
    }
}

/*
    generates random entry times for the IOs of a process
*/
void generateEntryTimes(int *arr, int length, int serviceTime) {
    for (int j = 0; j < length; j++) {
            int entryTime = 1 + (rand() % (serviceTime - 1));
            int found = FALSE;
            for (int k = 0; k < j; k++) {
                if (arr[k] == entryTime) {
                    found = TRUE;
                    break;
                }
            }
            if (found) {
                j--;
                continue;
            }
            arr[j] = entryTime;
        }

    //sort the entry times
    int aux;
    for (int i = 0; i < length; i++) {
        for (int j = i + 1; j < length; j++) {
            if (arr[i] > arr[j]) {
                aux = arr[i];
                arr[i] = arr[j];
                arr[j] = aux;
            }
        }
    }
}

/*
    create a random number of processes between 1 and MAX_PROCESSES, orders them by arrival time 
    and returns a pointer to the first process
*/
Process* createProcesses(int *numProcesses, Queues *queues) {
    int arrivalTime, serviceTime, IOCount;
    Process *processes = (Process*) malloc(sizeof(Process) * MAX_PROCESSES);
    if (processes == NULL) {
        printf("Error creating processes\n");
        exit(1);
    }
    Process *ptrProcess = processes;

    *numProcesses = 1 + (rand() % MAX_PROCESSES); //defines how many processes will be created

    for (int i = 0; i < *numProcesses; i++) {
        arrivalTime = i == 0 ? 0 : rand() % MAX_PROCESS_TIME;
        serviceTime = 1 + rand() % MAX_PROCESS_TIME;
        IOCount = (rand() % MAX_IO) % serviceTime;

        IO *ios = (IO *) malloc(sizeof(IO) * IOCount);
        if (ios == NULL) {
            printf("Error creating IOs\n");
            exit(1);
        }
        IO *ptrIO = ios;

        int arrivalTimes[IOCount];
        generateEntryTimes(arrivalTimes, IOCount, serviceTime);

        for (int j = 0; j < IOCount; j++) {
            IO io;
            int startTime = arrivalTimes[j];

            int ioType = rand() % 3;
            switch (ioType) {
                case 0:
                    strcpy(io.name, "disk");
                    io.duration = DISK;
                    break;
                case 1:
                    strcpy(io.name, "tape");
                    io.duration = TAPE;
                    break;
                case 2:
                    strcpy(io.name, "printer");
                    io.duration = PRINTER;
                    break;
                default:
                    printf("Error creating IOs\n");
                    exit(1);
                    break;
            }
            io.startTime = startTime;

            *ptrIO = io;
            ptrIO++;
        }

        *ptrProcess = newProcess(i+1, arrivalTime, serviceTime, IOCount, ios);
        ptrProcess++;
    }

    sortProcesses(processes, *numProcesses);
    return processes;
}

/*
    initialize the values for a queue struct
*/
Queue* initQueue() {
    Queue *queue = (Queue *) malloc(sizeof(Queue));
    if (queue == NULL) {
        printf("Error creating queue\n");
        exit(1);
    }
    queue->first = NULL;
    queue->last = NULL;

    return queue;
}

/*
    initialize the values for the scheduler struct
*/
void initScheduler(Scheduler *scheduler) {
    srand((unsigned) time(NULL));

    // creates the processes
    scheduler->processes = createProcesses(&scheduler->numProcesses, scheduler->queues);
    scheduler->TerminatedProcesses = 0;

    // creates the queues
    scheduler->queues = (Queues *) malloc(sizeof(Queues));
    if (scheduler->queues == NULL) {
        printf("Error creating queues\n");
        exit(1);
    }

    scheduler->queues->highPriority = initQueue();
    scheduler->queues->lowPriority = initQueue();
    scheduler->queues->disk = initQueue();
    scheduler->queues->tape = initQueue();
    scheduler->queues->printer = initQueue();

    // initializes the CPU
    scheduler->currentCPUProcess = NULL;
    scheduler->quantum = QUANTUM;
    scheduler->elapsedQuantum = 0;
}

/*
    main function to execute the simulator
*/
int main () {
    Scheduler *scheduler = (Scheduler *) malloc(sizeof(Scheduler));
    initScheduler(scheduler);

    // print each generated process pid and its IOs
    printf("Generated processes:\n");
    for (int i = 0; i < scheduler->numProcesses; i++) {
        // print process PID and IOnumber
        printf("Process %d: %d\n", scheduler->processes[i].pid, scheduler->processes[i].numIOs);
        for (int j = 0; j < scheduler->processes[i].numIOs; j++) {
            // print IOs for each process name, duration, and start time
            printf("IO %d: %s %d %d\n", j+1, scheduler->processes[i].ios[j].name, scheduler->processes[i].ios[j].duration, scheduler->processes[i].ios[j].startTime);
        }
    }
    return 0;
}