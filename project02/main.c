#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESSES 4 // maximum number of processes
#define MAX_PROCESS_TIME 15 // maximum time a process can take to finish
#define MAX_IO 3 // maximum number of IOs a process can have
#define QUANTUM 5

enum PROCESS_IO { DISK = 3, TAPE = 5, PRINTER = 8 };

enum PROCESS_STATE { READY, RUNNING, BLOCKED, FINISHED };

enum PROCESS_PRIORITY { HIGH, LOW };

// estruturas que precisaremos
typedef struct {
    int arrivalTime;
    int serviceTime;
    int processedTime;

    int pid;
    int ppid;
    int state;
    int priority;

    // TODO: IOs
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

    int processCount;
    int numProcesses;
} Scheduler;


/*
    creates a new process with the given parameters
*/
Process newProcess(int pid, int arrivalTime, int serviceTime) {
    Process process;

    process.pid = pid;
    process.state = READY;
    process.priority = HIGH;
    process.ppid = -1;
    process.arrivalTime = arrivalTime;
    process.serviceTime = serviceTime;
    process.processedTime = 0;

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
    create a random number of processes between 1 and MAX_PROCESSES, orders them by arrival time 
    and returns a pointer to the first process
*/
Process* createProcesses(int *numProcesses, Queues *queues) {
    int arrivalTime, serviceTime;
    Process *processes = (Process*) malloc(sizeof(Process) * MAX_PROCESSES);
    if (processes == NULL) {
        printf("Error creating processes\n");
        exit(1);
    }
    Process *ptrProcess = processes;

    srand((unsigned) time(NULL));

    *numProcesses = 1 + (rand() % MAX_PROCESSES); //defines how many processes will be created

    for (int i = 0; i < *numProcesses; i++) {
        arrivalTime = i == 0 ? 0 : rand() % MAX_PROCESS_TIME;
        serviceTime = 1 + rand() % MAX_PROCESS_TIME;

        *ptrProcess = newProcess(i+1, arrivalTime, serviceTime);
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
    // creates the processes
    scheduler->processes = createProcesses(&scheduler->numProcesses, scheduler->queues);
    scheduler->processCount = 0;

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


}

/*
    main function to execute the simulator
*/
int main () {
    Scheduler *scheduler = (Scheduler *) malloc(sizeof(Scheduler));
    initScheduler(scheduler);

    return 0;
}