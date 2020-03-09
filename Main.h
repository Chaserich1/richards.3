/* Author: Chase Richards
   Project: Homework 3 CS4760
   Date March 3, 2020
   Filename: Main.h  */

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

FILE* INFILE;

//Contains Help Message
void displayHelpMessage();

//Detaching and removing shared memory
int deallocateMem(int shmid, void *shmaddr);

//Getting and Attaching to shared Memory
void sharedMemoryWork(int totalInts, int n, char *inFile);

//Handling the fork and exec
void processHandler(int totalInts, int n);

//Signal Handling (ctrl c and timeout)
void sigHandler(int sig);

//Open the file and check for failure
FILE* openFile(char *filename, char *mode);

//Reading from input file and putting in shared memory
int readFile(FILE* filePtr, int sharedMemSegment);

#endif
