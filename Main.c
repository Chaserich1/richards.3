/* Author: Chase Richards
   Project: Homework 3 CS4760
   Date March 3, 2020
   Filename: Main.c  */

#include "Main.h"

int main(int argc, char* argv[])
{
    int c;
    char *inFile = "input.dat"; //Default inputfile name
    int n = 1; //Max Children in system at once
    int totalInts = 0;

    while((c = getopt(argc, argv, "hf:n:")) != -1)
    {
        switch(c)
        {
            case 'h':
                displayHelpMessage();
                return (EXIT_SUCCESS);
            case 'f':
                inFile = optarg;
                break;
            case 'n':
                n = atoi(optarg);
                break;
            default:
                printf("Using default values");
                break;               
        }
    }
  
    sharedMemoryWork(totalInts, n, inFile);

}

void sharedMemoryWork(int totalInts, int n, char *inFile)
{
    key_t key;
    int sharedMemSegment, sharedMemDetach;

    //Key returns a key based on the path and id
    key = ftok(".",'m');
    if(key == -1)
    {
        perror("master: Error: getting the shared memory key");
        exit(EXIT_FAILURE);
    }

    //Allocate the shared memory using shmget
    sharedMemSegment = shmget(key, sizeof(int), IPC_CREAT | 0644);
    
    //If shmget is unsuccessful it retruns -1 so check for this
    if(sharedMemSegment == -1) 
    {
        perror("master: Error: shmget failed to allocate shared memory");
        exit(EXIT_FAILURE);
    }

    //Attach to shared memory segment
    int *intArray = (int *)shmat(sharedMemSegment, (void *)0, 0);
    
    //If shmat returns -1 then it failed to attach
    if(intArray == (void *)-1)
    {
        perror("master: Error: Failed to attach shared memory for integer array");
        exit(EXIT_FAILURE);
    }

    INFILE = fopen(inFile, "a"); 
    
    //printf("%d %d\n", totalInts, n);
    processHandler(totalInts, n);

    //Detach and Remove Shared Memory
    sharedMemDetach = deallocateMem(sharedMemSegment, intArray); 
    
    //Check for shared memory detach to return -1
    if(sharedMemDetach == -1)
    {
        perror("master: Error: shared memory detach and removal failed");
        exit(EXIT_FAILURE);
    }
}

void processHandler(int totalInts, int n)
{
    int completedChildren = 0; //Completed child processes
    int runningChildren = 0; //Children in system
    int childCounter = 0;
    pid_t pid, waitingID;
    int status;
    int childExec; //For checking exec failure
    int index = 0;
    char strIndex[50]; //Convert index to char string for exec

    while(completedChildren < 64)
    {
        if(runningChildren < 20 && childCounter < 64)
        {
            //Fork
            pid = fork();
            runningChildren++;
            //Check for fork failure if it returns -1
            if(pid == -1)
            {
                perror("master: Error: Failed to fork");
                exit(EXIT_FAILURE);
            }
            //Fork returns 0 if it is successful
            else if(pid == 0)
            {
                sprintf(strIndex, "%d", completedChildren);
                char *args[] = {"./bin_adder", strIndex, NULL};
                childExec = execvp(args[0], args);

                //If exec fails it returns -1 so perror and exit
                if(childExec == -1)
                {
                    perror("master: Error: Child failed to exec");
                    exit(EXIT_FAILURE);
                }                                   
            }
            childCounter++;
        }
        
        waitingID = waitpid(-1, &status, WNOHANG);

        if(waitingID > 0)
        {
            completedChildren++;
            runningChildren--;
            //printf("%d\n", completedChildren);
        }
    }


}

//Shared Memory Deallocation and removal
int deallocateMem(int shmid, void *shmaddr) 
{
    //If detaching fails it will return -1 so return -1 for deallocation call
    if(shmdt(shmaddr) == -1)
        return -1;
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}

/* For the -h option that can be entered */
void displayHelpMessage() 
{
    printf("\n---------------------------------------------------------\n");
    printf("See below for the options:\n\n");
    printf("-h    : Instructions for running the project and terminate.\n");
    printf("-f filename  : Input file (Default: input.dat.\n"); 
    printf("-n x  : Number of children allowed to exist in the system at same time (Default: 1).\n");
    printf("\n---------------------------------------------------------\n");
    printf("Examples of how to run program(default and with options):\n\n");
    printf("$ make\n");
    printf("$ ./master\n");
    printf("$ ./master -f input.dat -n 10\n");
    printf("$ make clean\n");
    printf("\n---------------------------------------------------------\n"); 
    exit(0);
}
