/**************************************************************
* Class:  CSC-415
* Name: Jennifer Finaldi (Dreamteam)
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

void testOutput(dirEntry *rootDir);
long initDirectory(int parentLBA);

#endif