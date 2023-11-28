#ifndef STRUCTURES_H
#define STRUCTURES_H

#define TRUE 1
#define FALSE 0

#define MAX_PROCESSES 4 // maximum number of processes
#define MAX_PROCESS_TIME 15 // maximum time a process can take to finish
#define MAX_IO 3 // maximum number of IOs a process can have
#define QUANTUM 5 // quantum for the round robin algorithm

enum PROCESS_IO { DISK = 3, TAPE = 5, PRINTER = 8 }; // IO duration for each device

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

    int terminatedProcesses;
    int numProcesses;

    Process *currentCPUProcess;
    int quantum;
    int elapsedQuantum;
} Scheduler;

#endif