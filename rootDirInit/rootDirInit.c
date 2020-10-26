/**************************************************************
* Class:  CSC-415 Section #2 
* Name: Jennifer Finaldi (DreamTeam)
* Student ID: 920290420
* Project: File System
*
* File: rootDirInit.c
*
* Description: This function initializes a root directory
*
**************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <math.h>

#include "dirEntry.h"
//#include "bitMap.h"

#define NUM_BLOCKS 5  //how many blocks we will allocate by default
#define BLOCK_SIZE 512 //size of one LBA block
#define STARTING_NUM_DIR 50 //starting number of directories for array
#define ROOT_START_ID 1 //I guess 1 will be the very first starting id 
#define ROOT_OWNER_ID 1 //root owns itself?

extern void initDir(dirEntry*); //doesnt fix it

void rootDirInit()
{
	//get an address for the starting block
	//long startingBlock = find_free(NUM_BLOCKS);
	long startingBlock = 345; //TEMP hardcoded

	//create a space in RAM to start manipulating
	uint64_t* ptr = (uint64_t*)malloc(BLOCK_SIZE * NUM_BLOCKS);

	//create an instance of our directory entry
	dirEntry rootDir;
	initEntry(&rootDir); //initialize this root dir instance

	//assign the parent of this root dir to the starting block
	//assign the id of root dir
	//

	//initialize an array of directory entries, all set to unused
		//malloc memory for the whole array, sizeof struct * #directories
	//set self and parent entries
	//call initDirectory
	//call LBA write
}

int main() 
{
	rootDirInit();
}
