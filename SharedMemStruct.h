/* Author: Chase Richards
   Project: Homework 3 CS4760
   Date March 11, 2020
   Filename: SharedMemStruct.h  */

struct sharedMemory
{
    int integers[64];
    int calculationFlg[2];
};

struct sharedMemory *smPtr;
