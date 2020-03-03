/* Author: Chase Richards
   Project: Homework 3 CS4760
   Date March 3, 2020
   Filename: ChildProcess.h  */

#ifndef CHILDPROCESS_H
#define CHILDPROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>

//Deallocate shared memory
int deallocateMem(int shmid, void *shmaddr);

#endif
