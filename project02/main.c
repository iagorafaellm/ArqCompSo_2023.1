#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "headers/ansi_colors.h"
#include "headers/structures.h"

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

        //print created process info
        printf(ANSI_BOLD "\n\n========================== Process %d created: ==========================\n\n" ANSI_RESET, i+1);
        printf(ANSI_BOLD "arrival time: " ANSI_RESET "%d\n", arrivalTime);
        printf(ANSI_BOLD "service time: " ANSI_RESET "%d\n", serviceTime);
        printf(ANSI_BOLD "quantity of I/Os: " ANSI_RESET "%d\n", IOCount);
        for (int j = 0; j < IOCount; j++) {
            printf(">>> type: %s, start time: %d\n", ios[j].name, ios[j].startTime);
        }
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
    scheduler->terminatedProcesses = 0;

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
    Process arrival event
*/
void enterNewProcess(Process *processes, int numProcesses, Queues *queues, int currentTime) {
    for (int i = 0; i < numProcesses; i++) {
        if (processes[i].arrivalTime == currentTime) {
            printf(ANSI_GREEN "+" ANSI_RESET " Process %d arrived\n", processes[i].pid);
            // add the process to the high priority queue
            QueueElement *newElement = (QueueElement *) malloc(sizeof(QueueElement));
            if (newElement == NULL) {
                printf("Error creating new queue element\n");
                exit(1);
            }
            newElement->process = &processes[i];
            newElement->next = NULL;

           
            if (queues->highPriority->first == NULL) {
                queues->highPriority->first = newElement;
                queues->highPriority->last = newElement;
            } else {
                queues->highPriority->last->next = newElement;
                queues->highPriority->last = newElement;
            }
        }
    }
}

/*
    End of current CPU process event
*/
void terminateProcess(Process** currentCPUProcess, int *terminatedProcesses, int *elapsedQuantum) {
    if ((*currentCPUProcess)->processedTime == (*currentCPUProcess)->serviceTime) {
        printf(ANSI_RED "-" ANSI_RESET " Process %d terminated\n", (*currentCPUProcess)->pid);
        (*currentCPUProcess)->state = FINISHED;
        *elapsedQuantum = 0;
        (*terminatedProcesses)++;
        *currentCPUProcess = NULL;
        return;
    }
}

/*
    preemption of process event
*/
void preemptionProcess(int *elapsedQuantum, int quantum, Process** currentCPUProcess, Queue *highPriority, Queue *lowPriority) {
    if (*elapsedQuantum == quantum) {
        printf(ANSI_GREEN "+" ANSI_RESET " Process %d preempted\n", (*currentCPUProcess)->pid);
        (*currentCPUProcess)->state = READY;
        (*currentCPUProcess)->priority = LOW;

        QueueElement *newElement = (QueueElement *) malloc(sizeof(QueueElement));
        if (newElement == NULL) {
            printf("Error creating new queue element\n");
            exit(1);
        }
        newElement->process = *currentCPUProcess;
        newElement->next = NULL;

        if (lowPriority->first == NULL) {
            lowPriority->first = newElement;
            lowPriority->last = newElement;
        } else {
            lowPriority->last->next = newElement;
            lowPriority->last = newElement;
        }

        *currentCPUProcess = NULL;
        *elapsedQuantum = 0;
    }

}

/*
    IO call event
*/
void callIO(Process** currentCPUProcess, Queue* disk, Queue* tape, Queue* printer, int *elapsedQuantum) {
    if ((*currentCPUProcess)->currentIO == (*currentCPUProcess)->numIOs) {
        return;
    }

    IO io = (*currentCPUProcess)->ios[(*currentCPUProcess)->currentIO];
    if ((*currentCPUProcess)->processedTime == io.startTime) {
        printf(ANSI_GREEN "+" ANSI_RESET " Process %d called %s\n", (*currentCPUProcess)->pid, io.name);

        QueueElement *newElement = (QueueElement *) malloc(sizeof(QueueElement));
        if (newElement == NULL) {
            printf("Error creating new queue element\n");
            exit(1);
        }
        newElement->process = *currentCPUProcess;
        newElement->next = NULL;

        switch (io.duration) {
            case DISK:
                if (disk->first == NULL) {
                    disk->first = newElement;
                    disk->last = newElement;
                } else {
                    disk->last->next = newElement;
                    disk->last = newElement;
                }
                break;
            case TAPE:
                if (tape->first == NULL) {
                    tape->first = newElement;
                    tape->last = newElement;
                } else {
                    tape->last->next = newElement;
                    tape->last = newElement;
                }
                break;
            case PRINTER:
                if (printer->first == NULL) {
                    printer->first = newElement;
                    printer->last = newElement;
                } else {
                    printer->last->next = newElement;
                    printer->last = newElement;
                }
                break;
            default:
                printf("Error calling IO\n");
                exit(1);
                break;
        }

        (*currentCPUProcess)->state = BLOCKED;
        (*currentCPUProcess) = NULL;
        *elapsedQuantum = 0;
    }
}

/*
    Advance the current CPU process event
*/
void advanceCPUProcess(Process** currentCPUProcess, Queues *queues, int *terminatedProcesses, int quantum, int *elapsedQuantum) {
    if (*currentCPUProcess == NULL) {
        if (queues->highPriority->first != NULL) {
            *currentCPUProcess = queues->highPriority->first->process;
            queues->highPriority->first = queues->highPriority->first->next;
        } else if (queues->lowPriority->first != NULL) {
            *currentCPUProcess = queues->lowPriority->first->process;
            queues->lowPriority->first = queues->lowPriority->first->next;
        } else {
            printf(ANSI_YELLOW "*" ANSI_RESET " No process to run\n");
            return;
        }

        (*currentCPUProcess)->state = RUNNING;
    }

    printf(ANSI_YELLOW "*" ANSI_RESET " Process %d is running\n", (*currentCPUProcess)->pid);
    (*currentCPUProcess)->processedTime++;
    (*elapsedQuantum)++;

    terminateProcess(currentCPUProcess, terminatedProcesses, elapsedQuantum);

    if (*currentCPUProcess != NULL) callIO(currentCPUProcess, queues->disk, queues->tape, queues->printer, elapsedQuantum);
    if (*currentCPUProcess != NULL) preemptionProcess(elapsedQuantum, quantum, currentCPUProcess, queues->highPriority, queues->lowPriority);
}

/*
    Advance the current IO processes event
*/
void advanceIOProcess(Queues *queues) {
    // Disk queue
    if (queues->disk->first != NULL) {
        queues->disk->first->process->elapsedIOTime++;
        if (queues->disk->first->process->elapsedIOTime == DISK) {
            printf(ANSI_GREEN "+" ANSI_RESET " Process %d finished disk IO\n", queues->disk->first->process->pid);
            queues->disk->first->process->currentIO++;
            queues->disk->first->process->state = READY;
            queues->disk->first->process->priority = LOW;
            queues->disk->first->process->elapsedIOTime = 0;

            QueueElement *newElement = (QueueElement *) malloc(sizeof(QueueElement));
            if (newElement == NULL) {
                printf("Error creating new queue element\n");
                exit(1);
            }
            newElement->process = queues->disk->first->process;
            newElement->next = NULL;

            if (queues->lowPriority->first == NULL) {
                queues->lowPriority->first = newElement;
                queues->lowPriority->last = newElement;
            } else {
                queues->lowPriority->last->next = newElement;
                queues->lowPriority->last = newElement;
            }

            queues->disk->first = queues->disk->first->next;
        }
    }

    // Tape queue
    if (queues->tape->first != NULL) {
        queues->tape->first->process->elapsedIOTime++;
        if (queues->tape->first->process->elapsedIOTime == TAPE) {
            printf(ANSI_GREEN "+" ANSI_RESET " Process %d finished tape IO\n", queues->tape->first->process->pid);
            queues->tape->first->process->currentIO++;
            queues->tape->first->process->state = READY;
            queues->tape->first->process->priority = HIGH;
            queues->tape->first->process->elapsedIOTime = 0;

            QueueElement *newElement = (QueueElement *) malloc(sizeof(QueueElement));
            if (newElement == NULL) {
                printf("Error creating new queue element\n");
                exit(1);
            }
            newElement->process = queues->tape->first->process;
            newElement->next = NULL;

            if (queues->highPriority->first == NULL) {
                queues->highPriority->first = newElement;
                queues->highPriority->last = newElement;
            } else {
                queues->highPriority->last->next = newElement;
                queues->highPriority->last = newElement;
            }

            queues->tape->first = queues->tape->first->next;
        }
    }

    // Printer queue
    if (queues->printer->first != NULL) {
        queues->printer->first->process->elapsedIOTime++;
        if (queues->printer->first->process->elapsedIOTime == PRINTER) {
            printf(ANSI_GREEN "+" ANSI_RESET " Process %d finished printer IO\n", queues->printer->first->process->pid);
            queues->printer->first->process->currentIO++;
            queues->printer->first->process->state = READY;
            queues->printer->first->process->priority = HIGH;
            queues->printer->first->process->elapsedIOTime = 0;

            QueueElement *newElement = (QueueElement *) malloc(sizeof(QueueElement));
            if (newElement == NULL) {
                printf("Error creating new queue element\n");
                exit(1);
            }
            newElement->process = queues->printer->first->process;
            newElement->next = NULL;

            if (queues->highPriority->first == NULL) {
                queues->highPriority->first = newElement;
                queues->highPriority->last = newElement;
            } else {
                queues->highPriority->last->next = newElement;
                queues->highPriority->last = newElement;
            }

            queues->printer->first = queues->printer->first->next;
        }
    }
}

/*
    free all the allocated memory
*/
void freeStructures(Scheduler *scheduler) {
    for (int i = 0; i < scheduler->numProcesses; i++) {
        free(scheduler->processes[i].ios);
    }
    free(scheduler->processes);
    free(scheduler->queues->highPriority);
    free(scheduler->queues->lowPriority);
    free(scheduler->queues->disk);
    free(scheduler->queues->tape);
    free(scheduler->queues->printer);
    free(scheduler->queues);
}

/*
    print all queues and their processes
*/
void printQueues(Queues *queues) {
    printf("\nHigh priority queue: ");
    QueueElement *ptr = queues->highPriority->first;
    while (ptr != NULL) {
        printf("%d ", ptr->process->pid);
        ptr = ptr->next;
    }
    printf("\nLow priority queue: ");
    ptr = queues->lowPriority->first;
    while (ptr != NULL) {
        printf("%d ", ptr->process->pid);
        ptr = ptr->next;
    }
    printf("\nDisk queue: ");
    ptr = queues->disk->first;
    while (ptr != NULL) {
        printf("%d ", ptr->process->pid);
        ptr = ptr->next;
    }
    printf("\nTape queue: ");
    ptr = queues->tape->first;
    while (ptr != NULL) {
        printf("%d ", ptr->process->pid);
        ptr = ptr->next;
    }
    printf("\nPrinter queue: ");
    ptr = queues->printer->first;
    while (ptr != NULL) {
        printf("%d ", ptr->process->pid);
        ptr = ptr->next;
    }
    printf("\n");
}

/*
    print information about the current state of the CPU
*/
void printCPU(Scheduler *scheduler){
    //print qtd of terminated processes / all processes, elapsed_time / service_time and elapsed_quantum / quantum
    printf("\nCPU: ");
    printf("terminated processes: %d/%d, ", scheduler->terminatedProcesses, scheduler->numProcesses);
    if (scheduler->currentCPUProcess != NULL) {
        printf("processed time: %d/%d, ", scheduler->currentCPUProcess->processedTime, scheduler->currentCPUProcess->serviceTime);
    } else {
        printf("processed time: 0/0, ");
    }
    printf("elapsed quantum: %d/%d\n", scheduler->elapsedQuantum, scheduler->quantum);
}

/*
    simulates the scheduler
*/
void simulate(Scheduler *scheduler) {
    int currentTime = 0;
    while (scheduler->terminatedProcesses < scheduler->numProcesses) {
        printf("\n\n========================== " ANSI_BOLD "Current time: %d" ANSI_RESET " ==========================\n", currentTime);
        
        enterNewProcess(scheduler->processes, scheduler->numProcesses, scheduler->queues, currentTime);
        advanceCPUProcess(&(scheduler->currentCPUProcess), scheduler->queues, &(scheduler->terminatedProcesses), scheduler->quantum, &(scheduler->elapsedQuantum));
        advanceIOProcess(scheduler->queues);
        printQueues(scheduler->queues);
        printCPU(scheduler);

        currentTime++;
    }
}

/*
    main function to execute the simulator
*/
int main () {
    Scheduler *scheduler = (Scheduler *) malloc(sizeof(Scheduler));
    initScheduler(scheduler);
    simulate(scheduler);
    freeStructures(scheduler);
    return 0;
}