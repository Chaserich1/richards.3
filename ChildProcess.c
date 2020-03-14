/* Author: Chase Richards
   Project: Homework 3 CS4760
   Date March 3, 2020
   Filename: ChildProcess.c  */

#include "ChildProcess.h"

int main(int argc, char* argv[])
{
    key_t key;
    int sharedMemSegment, sharedMemDetach;
 
    //ftok gives me the key based path and 'm' id
    key = ftok(".",'a');

    //If key returns -1 then it failed so check
    if(key == -1)
    {
        perror("bin_adder: Error: Failed to get shared memory key");
        exit(EXIT_FAILURE);
    }
    
    //Allocate the shared memory using shmget
    sharedMemSegment = shmget(key, sizeof(struct sharedMemory), IPC_CREAT | 0666);
    
    //If shmget returns -1 then it failed
    if(sharedMemSegment == -1)
    {
        perror("bin_adder: Error: Failed to get shared memory segment");
        exit(EXIT_FAILURE);
    }
    
    //Attach to the shared memory segment
    smPtr = shmat(sharedMemSegment, (void *)0, 0);

    //Check if it returns -1
    if(smPtr == (void *) -1)
    {
        perror("bin_adder: Error: Failed to attach to shared memory");
        exit(EXIT_FAILURE);
    }   
 
    int index = atoi(argv[1]);
    int numIntsToAdd = atoi(argv[2]);

    //printf("%d, %d, %d\n", index, numIntsToAdd, smPtr-> integers[index]);
    
    firstCriticalSection(index, numIntsToAdd);  

    //Detach from shared memory and check for it returning -1
    sharedMemDetach = deallocateMem(sharedMemSegment, (void*) smPtr);
    if(sharedMemDetach == -1)
    {
        perror("bin_adder: Error: Failed to detach shared memory");
        exit(EXIT_FAILURE);
    }
    return 0;
}

// n/2 partition and critical section
void firstCriticalSection(int index, int numIntsToAdd)
{
    FILE* file;
    file = fopen("adder_log", "a");
    
    //Semaphore Declarations
    sem_t *sem;
    char *semaphoreName = "semChild";
    //Open semaphore and check for failure
    sem = sem_open(semaphoreName, 0);
    if(sem == SEM_FAILED)
    {
        perror("bin_adder: Error: Failed to open semaphore");
        exit(EXIT_FAILURE);
    }

    int i;
    for(i = 1; i < numIntsToAdd; i++)
    {
        smPtr-> integers[index] += smPtr-> integers[index + i];
        smPtr-> integers[index + i] = 0;
    }

    //Critical Section
    int k;
    char time[10];
    for(k = 0; k < numIntsToAdd / 2; k++)
    {
        sleep(rand() % 4); //sleep for somewhere between 0-3 seconds
        timeSetter(time);
        fprintf(stderr, "%d waiting to enter the critical section at %s\n", getpid(), time);
        sem_wait(sem); //waiting for its time to enter critcal section
        //Beginning Critical Section
        timeSetter(time);
        fprintf(stderr, "%d is inside the critical section at %s\n", getpid(), time);
        sleep(1); //Sleep another 1 second before writing to file
        //TO DO: Open file, write to file
        fprintf(file, "\t%d\t\t%d\t\t%d\t\t%d\n", getpid(), index, numIntsToAdd, smPtr-> integers[index]);       
        sleep(1); //Sleep another 1 second before leaving critical section
        timeSetter(time);
        fprintf(stderr, "%d is exiting the critical section at %s\n", getpid(), time);
        sem_post(sem); //Critical section finished, signal semaphore   
    }
}

//Deallocate shared memory but don't remove it
int deallocateMem(int shmid, void *shmaddr) 
{
    //If detaching fails it will return -1 so return -1 for deallocation call
    if(shmdt(shmaddr) == -1)
        return -1;
    //shmctl(shmid, IPC_RMID, NULL); Don't need to remove the shared memory
    return 0;
}

//Set the values for the current time to be indicated on stderr
void timeSetter(char *tStr)
{
    time_t ttime;
    struct tm *timeStruct;
    time(&ttime);
    timeStruct = localtime(&ttime);
    sprintf(tStr, "%02d:%02d:%02d", timeStruct-> tm_hour, timeStruct-> tm_min, timeStruct-> tm_sec);
         
}
