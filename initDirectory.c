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
	printf("I made it inside initDirectory line 18\n");
	int i = 0; //iterator for array of entries

	printf("sizeof(dirEntry): %ld\n", sizeof(dirEntry));
	printf("numBlocks = %ld\n", MBR_st->dirNumBlocks);

	//get an address for the starting block
	int startingBlock = find_free_index(MBR_st->dirNumBlocks);
	//int startingBlock = 50; 

	printf("I got my starting block at: %d\n", startingBlock);
	//create a space in RAM to start manipulating
	dirEntry* ptr = (dirEntry*)malloc(MBR_st->dirBufMallocSize);
	if (ptr == NULL)
	{
		printf("Error with malloc ln50.\n");
		return -1;
	}
	else
		printf("Malloc succeeded\n\n");

	//fill the root struct with default info
	initEntry(&ptr[i]); //initialize this root dir instance
	ptr[i].locationLBA = startingBlock;
	ptr[i].entryIndex = 0;
	ptr[i].name[0] = '.';
	ptr[i].name[1] = '\0';
	ptr[i].isBeingUsed = 1; 
	i++; //increment to next directory
	
	//testOutput(&ptr[0]); //print this first root entry

	//initialize an array of directory entries, all set to unused
	for (i; i < STARTING_NUM_DIR; i++)
	{
		initEntry(&ptr[i]);

		//initialize the .. entry
		if(i < 2) {
			// if(parentLBA == 0)
			// 	ptr[i].locationLBA = startingBlock; //.. points to starting block
			// else ptr[i].locationLBA = parentLBA; //.. points to parentLBA of parent dir

			//set the second entry's name
			ptr[i].name[0] = '.';
			ptr[i].name[1] = '.';
			ptr[i].name[2] = '\0';
			
			ptr[i].isBeingUsed = 1; 
		}
		ptr[i].entryIndex = i;
		if(parentLBA == 0)
			ptr[i].locationLBA = startingBlock; //.. points to starting block
		else ptr[i].locationLBA = parentLBA;
		
		//MBR_st->idCounter++;
		//ptr[i].id = MBR_st->idCounter;

		//testOutput(&ptr[i]); //print out the individual entry just created
	}
	//ptr[6].locationLBA = 1700;
	//dirEntry *buff = (dirEntry *)malloc(BLOCK_SIZE * numBlocks);
	//call LBA write to put this directory on disk
	 LBAwrite(ptr, MBR_st->dirNumBlocks, startingBlock);
	//LBAread (buff, numBlocks, startingBlock); 
	//printf("HELLLLOOOO %d", buff[6].locationLBA); 

	//free(ptr); 

	printf("root directory successfully initiated.\n\n");

	return startingBlock; //returning the location of the directory in LBA
}

void testOutput(dirEntry *rootDir)
{
	//print out its contents
	printf("\n\n---- Directory Contents ----\n");
	printf("Location of Self: %ld\n", rootDir->locationLBA);
	printf("Data Location: %ld\n", rootDir->dataLocation);
	printf("Child Location: %ld\n", rootDir->childLBA);
	printf("Name: %s\n", rootDir->name);
	printf("Index: %ld\n", rootDir->entryIndex);
	printf("Size of File: %ld\n", rootDir->sizeOfFile);
	printf("Number of Blocks: %ld\n", rootDir->numBlocks);

	struct tm *timeInfo = localtime(&(rootDir->dateModifiedDirectory));
	printf("DateModified: %s", asctigit me(timeInfo));
	timeInfo = localtime(&(rootDir->dateAccessedDirectory));
	printf("DateAccessed: %s", asctime(timeInfo));

	printf("Location of Metadata: %ld\n", rootDir->locationMetadata);
	printf("isBeingUsed: %s\n\n", rootDir->isBeingUsed ? "true" : "false");
}

/*int main() 
{
	printf("rootDirInit executed without errors: %s\n\n", initDirectory(0) > 0 ? "true" : "false");
}*/