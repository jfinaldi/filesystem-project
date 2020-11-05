/**************************************************************
* Class:  CSC-415 Section #2 
* Name: Jennifer Finaldi (DreamTeam)
* Student ID: 920290420
* Project: File System
*
* File: initDirectory.c
*
* Description: This function initializes an entire directory
*            and saves it on disk
*
**************************************************************/

#include "mfs.h" //for LBAWrite()

long initDirectory(int parentLBA)
{
	//calculate the number of blocks
	long structSize = sizeof(dirEntry);
	printf("sizeof(dirEntry): %ld\n", sizeof(dirEntry));
	long bytes = structSize * STARTING_NUM_DIR;
	printf("bytes = %ld\n", bytes);
	long numBlocks = 1 + (bytes / BLOCK_SIZE);
	printf("numBlocks = %ld\n", numBlocks);
	int trackOffset = 0;  //keeps track of where we are in each block
	int currentBlock = 0; //keeps track of which LBA block we're in

	//get an address for the starting block
	int startingBlock = find_free_index(numBlocks);
	//long startingBlock = 50; //TEMP hardcoded
	currentBlock = startingBlock;

	//create a space in RAM to start manipulating
	dirEntry *ptr = (dirEntry *)malloc(BLOCK_SIZE * numBlocks);
	if (ptr == NULL)
	{
		printf("Error with malloc ln50.\n");
		return -1;
	}
	else
		printf("Malloc succeeded\n\n");

	//fill the root struct with default info
	initEntry(&ptr[0]); //initialize this root dir instance
	printf("root directory successfully initiated.\n\n");

	//if the parentLBA is 0, then we are creating the very first root entry
	if (parentLBA == 0)
	{
		//assign the location LBA of self
		ptr[0].locationSelf = startingBlock;

		//assign the root dir
		ptr[0].rootDir = startingBlock;

		//assign the name of root dir
		ptr[0].name[0] = '.';

		//assign the name of root dir parent
		ptr[0].root[0] = '.'; //for any other, it would be ..
	}

	trackOffset += (int)sizeof(dirEntry); //move the tracker to the end of this struct
	if ((trackOffset / BLOCK_SIZE) > currentBlock)
	{
		currentBlock++;			   //we're crossing into a new block
		trackOffset -= BLOCK_SIZE; //now we know how far into the next block we are
	}
	//testOutput(&ptr[0]); //print this first root entry

	//initialize an array of directory entries, all set to unused
	for (int i = 1; i < STARTING_NUM_DIR; i++)
	{
		initEntry(&ptr[i]);

		//only the first and second entries get . and .. assigned
		if (i < 2)
		{
			//set the second entry's parent name
			ptr[i].root[0] = '.';
			ptr[i].root[1] = '.';
			ptr[i].root[2] = '\0';

			//set the second entry's name
			ptr[i].name[0] = '.';
			ptr[i].name[1] = '\0';
		}

		//calculate which block this entry starts in
		if (((trackOffset / BLOCK_SIZE) + startingBlock) > currentBlock)
			currentBlock++; //we're crossing into a new block
		trackOffset += (int)sizeof(dirEntry);

		//record location data in entry
		ptr[i].rootDir = startingBlock; //all entries should be able to find their root
		ptr[i].locationSelf = currentBlock;

		//testOutput(&ptr[i]); //print out the individual entry just created
	}

	//call LBA write to put this directory on disk
	LBAwrite(ptr, numBlocks, startingBlock);



	//free(ptr); //just for testing purposes

	return startingBlock; //returning the location of the directory in LBA
}

void testOutput(dirEntry *rootDir)
{
	//print out its contents
	printf("\n\n---- Directory Contents ----\n");
	printf("rootDir = %ld\n", rootDir->rootDir);
	//printf("ID: %ld\n", rootDir->id);
	printf("Location of Self: %ld\n", rootDir->locationSelf);
	printf("Data Location: %ld\n", rootDir->dataLocation);
	printf("Name: %s\n", rootDir->name);
	printf("Root Name: %s\n", rootDir->root);
	//printf("Permissions: [");
	//for(int i = 0; i < 3; i++)
	//	printf("%d", rootDir->permission[i]);
	//printf("]\n");
	//printf("Owner ID: %d\n", rootDir->owner);
	printf("Size of File: %ld\n", rootDir->sizeOfFile);
	printf("Number of Blocks: %ld\n", rootDir->numBlocks);
	//printf("Parent Block: %ld\n", rootDir->parent);

	struct tm *timeInfo = localtime(&(rootDir->dateModifiedDirectory));
	printf("DateModified: %s", asctime(timeInfo));
	timeInfo = localtime(&(rootDir->dateAccessedDirectory));
	printf("DateAccessed: %s", asctime(timeInfo));

	printf("Location of Metadata: %ld\n", rootDir->locationMetadata);
	printf("isBeingUsed: %s\n\n", rootDir->isBeingUsed ? "true" : "false");
}

/*int main() 
{
	printf("rootDirInit executed without errors: %s\n\n", initDirectory(0) > 0 ? "true" : "false");
}*/