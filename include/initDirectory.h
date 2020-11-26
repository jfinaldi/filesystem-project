/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: initDirectory.h
*
* Description: 
*	This is the header file for initDirectory
*
**************************************************************/

#ifndef _INITDIRECTORY_H
#define _INITDIRECTORY_H

#include "mfs.h"

#define STARTING_NUM_DIR 50 //starting number of directories for array
//#define BLOCK_SIZE 512      //the block size

void testOutput(dirEntry *rootDir);
long initDirectory(int parentLBA);

#endif