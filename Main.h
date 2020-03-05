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

#endif
