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
    key = ftok(".",'m');

    //If key returns -1 then it failed so check
    if(key == -1)
    {
        perror("bin_adder: Error: Failed to get shared memory key");
        exit(EXIT_FAILURE);
    }
    
    //Allocate the shared memory using shmget
    sharedMemSegment = shmget(key, sizeof(int), IPC_CREAT | 0644);
    
    //If shmget returns -1 then it failed
    if(sharedMemSegment == -1)
    {
        perror("bin_adder: Error: Failed to get shared memory segment");
        exit(EXIT_FAILURE);
    }
    
    //Attach to the shared memory segment
    int *sharedArr = (int *)shmat(sharedMemSegment, (void *)0, 0);

    //Check if it returns -1
    if(sharedArr == (void *) -1)
    {
        perror("bin_adder: Error: Failed to attach to shared memory");
        exit(EXIT_FAILURE);
    }

    //printf("ChildTest\n");  
 
    //Detach from shared memory and check for it returning -1
    sharedMemDetach = deallocateMem(sharedMemSegment, sharedArr);
    if(sharedMemDetach == -1)
    {
        perror("bin_adder: Error: Failed to detach shared memory");
        exit(EXIT_FAILURE);
    }

    return 0;
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
