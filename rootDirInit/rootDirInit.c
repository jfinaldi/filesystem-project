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

//#define NUM_BLOCKS 5  //how many blocks we will allocate by default
#define BLOCK_SIZE 512 //size of one LBA block
#define STARTING_NUM_DIR 50 //starting number of directories for array
#define ROOT_START_ID 1 //I guess 1 will be the very first starting id 
#define ROOT_OWNER_ID 1 //root owns itself?

extern void initDir(dirEntry*); 
void testOutput(dirEntry* rootDir);

int rootDirInit()
{
	//calculate the number of blocks
	long structSize = sizeof(dirEntry);
	printf("sizeof(dirEntry): %ld\n", sizeof(dirEntry));
	long bytes = structSize * STARTING_NUM_DIR;
	printf("bytes = %ld\n", bytes);
	long numBlocks = 1 + (bytes / BLOCK_SIZE);
	printf("numBlocks = %ld\n", numBlocks);

	//get an address for the starting block
	//long startingBlock = find_free(numBlocks);
	long startingBlock = 345; //TEMP hardcoded

	//create a space in RAM to start manipulating
	//dirEntry* ptr = (dirEntry*)malloc(BLOCK_SIZE * numBlocks);
	BLOCK_SIZE* ptr = malloc(BLOCK_SIZE * numBlocks);
	if(ptr == NULL) 
	{
		printf("Error with malloc ln50.\n");
		return 1;
	} else printf("Malloc succeeded\n\n");

	//create an instance of our directory entry
	dirEntry* rootDir  = ptr;
	initEntry(rootDir); //initialize this root dir instance
	printf("root directory successfully initiated.\n\n");

	//assign the parent of this root dir to the starting block
	rootDir->parent = startingBlock;

	//assign the id of root dir, same as parent
	rootDir->id = startingBlock;

	//assign the root dir
	rootDir->rootDir = startingBlock;
	
	//initialize an array of directory entries, all set to unused
	long blockNum = startingBlock;
	printf("ptr: %p\n", ptr);
	printf("sizeof(ptr): %ld\n", sizeof(ptr));
	ptr++; //move to the next block in memory
	printf("ptr: %p\n", ptr);
	printf("starting loop to initiate directory array...\n");
	for(ptr; ptr != NULL; ptr += BLOCK_SIZE, blockNum++)
	{
		dirEntry* anEntry = ptr;  printf("[%ld]anEntry ptr: %p\n", blockNum, anEntry);
		initEntry(anEntry);
		anEntry->locationSelf = blockNum;
	}
	
	//set self and parent entries
	//call initDirectory
	//call LBA write

	testOutput(rootDir);

	for(dirEntry* ptr = rootDir; ptr != NULL; ptr += BLOCK_SIZE)
	{
		testOutput(ptr);
	}


	free(rootDir); //just for testing purposes

	return 0; //no errors
}

void testOutput(dirEntry* rootDir)
{
	//print out its contents
	printf("\n\n---- Directory Contents ----\n");
	printf("rootDir = %ld\n", rootDir->rootDir);
	printf("ID: %ld\n", rootDir->id);
	printf("Location of Self: %ld\n", rootDir->locationSelf);
	printf("Data Location: %ld\n", rootDir->dataLocation);
	printf("Name: %s\n", rootDir->name);
	printf("Permissions: [");
	for(int i = 0; i < 3; i++)
		printf("%d", rootDir->permission[i]);
	printf("]\n");
	printf("Owner ID: %d\n", rootDir->owner);
	printf("Size of File: %ld\n", rootDir->sizeOfFile);
	printf("Number of Blocks: %ld\n", rootDir->numBlocks);
	printf("Parent Block: %ld\n", rootDir->parent);
	
	struct tm * timeInfo = localtime(&(rootDir->dateModifiedDirectory));
	printf("DateModified: %s", asctime(timeInfo));
	timeInfo = localtime(&(rootDir->dateAccessedDirectory));
	printf("DateAccessed: %s", asctime(timeInfo));

	printf("Location of Metadata: %ld\n", rootDir->locationMetadata);
	printf("isBeingUsed: %s\n\n", rootDir->isBeingUsed ? "true" : "false");	
}

int main() 
{
	//rootDirInit();
	printf("rootDirInit executed without errors: %s\n\n", rootDirInit() ? "false" : "true");
}
