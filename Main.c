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
  
    /* Signal for terminating, freeing up shared mem, killing all children 
       if the program goes for more than two seconds of real clock */
    signal(SIGALRM, sigHandler);
    alarm(100);

    /* Signal for terminating, freeing up shared mem, killing all 
       children if the user enters ctrl-c */
    signal(SIGINT, sigHandler);  

    sharedMemoryWork(totalInts, n, inFile);

}

void sharedMemoryWork(int totalInts, int n, char *inFile)
{
    key_t key;
    int sharedMemSegment, sharedMemDetach;
    int totalInts1;

    //Key returns a key based on the path and id
    key = ftok(".",'m');
    if(key == -1)
    {
        perror("master: Error: getting the shared memory key");
        exit(EXIT_FAILURE);
    }

    //Allocate the shared memory using shmget
    sharedMemSegment = shmget(key, sizeof(struct sharedMemory), IPC_CREAT | 0666);
    
    //If shmget is unsuccessful it retruns -1 so check for this
    if(sharedMemSegment == -1) 
    {
        perror("master: Error: shmget failed to allocate shared memory");
        exit(EXIT_FAILURE);
    }

    //Attach to shared memory segment
    smPtr = shmat(sharedMemSegment, (void *)0, 0);
    
    //If shmat returns -1 then it failed to attach
    if(smPtr == (void *)-1)
    {
        perror("master: Error: Failed to attach shared memory for integer array");
        exit(EXIT_FAILURE);
    }

    //Open the input file using the function that checks for failure
    INFILE = openFile(inFile, "r", n); 

    //Read the input file to shared memory
    totalInts = readFile(inFile, sharedMemSegment, n);

    writeLogHeaders();

    //printf("%d %d\n", totalInts, n);
    processHandler(totalInts, n);

    //Detach and Remove Shared Memory
    sharedMemDetach = deallocateMem(sharedMemSegment, (void*) smPtr); 
    
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
    char xx[20]; //Convert index to char string for exec
    char yy[20];
    //Semaphore Declarations
    sem_t* sem;
    char *semaphoreName = "semChild";

    //Create the semaphore with starting value of 1 and check for success
    sem = sem_open(semaphoreName, O_CREAT, 0644, 1);
    if(sem == SEM_FAILED)
    {
        perror("master: Error: Unable to open semaphore");
        exit(EXIT_FAILURE);
    }
   
    
 
    while(completedChildren < n)
    {
        if(runningChildren < 20 && childCounter < n)
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
                sprintf(xx, "%d", childCounter);
                sprintf(yy, "%d", totalInts);
                char *args[] = {"./bin_adder", xx, yy, NULL};
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
    //Destroy the semaphore
    sem_unlink(semaphoreName);

}

//Open the input file
FILE* openFile(char *fileName, char *mode, int n)
{
    FILE* filePtr = fopen(fileName, "w");
    if(filePtr == NULL)
    {
        perror("master: Error: Failed to open input file");
        exit(EXIT_FAILURE);
    }
   
    //Generate the random values for the input file between 0-256
    srand(time(0));
    int i;
    for(i = 0; i < n; i++)
    {
        int randValue;
        randValue = (rand() % (256 - 0 + 1) + 0);
        fprintf(filePtr, "%d\n", randValue);
    }
    fclose(filePtr);
 
    return filePtr;
}

//Read the integer values from the file and save to shared memory array
int readFile(char *fileName, int shmid, int n)
{
    //Open the file and check for failure
    FILE* filePtr = fopen(fileName, "r");
    if(filePtr == NULL)
    {
        perror("master: Error: Failed to open input file");
        exit(EXIT_FAILURE);
    }

    int count = 0;
    //Array for the file integers attached to shared memory
    int *integers;
    integers = shmat(shmid, NULL, 0);
    //Check if it returns -1
    if(integers < 0)
    {
        perror("master: Error: Failed to attach to sharedmemory");
        exit(EXIT_FAILURE);
    } 

    //Loop through the file and read the integers to the intergers array
    int i; 
    for(i = 0; i < n; i++)
    {
        fscanf(filePtr, "%d", &smPtr-> integers[i]);
        //printf("%d\n", smPtr-> integers[i]);
        count++;
    } 

    fclose(filePtr);
    //Detach from shared memory
    shmdt((void*) smPtr);
    return count;
}

void writeLogHeaders()
{
    FILE* logFile = fopen("adder_log", "a");
    if(logFile == NULL)
    {
        perror("master: Error: failed to open adder_log file");
        exit(EXIT_FAILURE);
    }
    
    fprintf(logFile, "\n----------------------------------------------------\n");
    fprintf(logFile, "\tPID\t\tIndex\t\tSize\n");
    fclose(logFile); 
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

/* Signal handler, that looks to see if the signal is for 2 seconds being up or ctrl-c being entered.
   In both cases, I connect to shared memory so that I can write the time that it is killed to the file
   and so that I can disconnect and remove the shared memory. */
void sigHandler(int sig)
{
    if(sig == SIGALRM)
    {
        key_t key = ftok(".",'m');
        int sharedMemSegment;
        sharedMemSegment = shmget(key, sizeof(int), IPC_CREAT | 0644);
        printf("One-Hundred Seconds is up.\n"); 
        printf("Killing children, removing shared memory and unlinking semaphore.\n");
        shmctl(sharedMemSegment, IPC_RMID, NULL);
        sem_unlink("semChild");
        kill(0, SIGKILL);
        exit(EXIT_SUCCESS);
    }
    
    if(sig == SIGINT)
    {
        key_t key = ftok(".",'m');
        int sharedMemSegment;
        sharedMemSegment = shmget(key, sizeof(int), IPC_CREAT | 0644);
        printf("Ctrl-c was entered\n");
        printf("Killing children, removing shared memory and unlinking semaphore\n");
        shmctl(sharedMemSegment, IPC_RMID, NULL);
        sem_unlink("semChild");
        kill(0, SIGKILL);
        exit(EXIT_SUCCESS);
    }
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
