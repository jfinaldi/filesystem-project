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
	dirEntry *ptr = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
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
	ptr[i].childLBA = startingBlock;
	ptr[i].entryIndex = 0;
	ptr[i].name[0] = 'Q';
	ptr[i].name[1] = '\0';
	ptr[i].isBeingUsed = 1;
	ptr[i].type = atoi("d");
	i++; //increment to next directory

	//testOutput(&ptr[0]); //print this first root entry

	//initialize an array of directory entries, all set to unused
	for (i; i < STARTING_NUM_DIR; i++)
	{
		initEntry(&ptr[i]);

		//initialize the .. entry
		if (i < 2)
		{
			// if(parentLBA == 0)
			// 	ptr[i].locationLBA = startingBlock; //.. points to starting block
			// else ptr[i].locationLBA = parentLBA; //.. points to parentLBA of parent dir

			//set the second entry's name
			ptr[i].name[0] = 'Q';
			ptr[i].name[1] = 'Q';
			ptr[i].name[2] = '\0';
			if (parentLBA == 0)
			{
				ptr[i].childLBA = startingBlock;
			}
			else
			{
				ptr[i].childLBA = parentLBA;
			}
			ptr[i].isBeingUsed = 1;
		}
		ptr[i].entryIndex = i;
		ptr[i].locationLBA = startingBlock;
		ptr[i].type = atoi("d");
		//MBR_st->idCounter++;
		//ptr[i].id = MBR_st->idCounter;

		//testOutput(&ptr[i]); //print out the individual entry just created
	}
	
	//call LBA write to put this directory on disk
	//dirEntry *ptr2 = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
	LBAwrite(ptr, MBR_st->dirNumBlocks, startingBlock);
	// LBAread(ptr2, MBR_st -> dirNumBlocks, 38); 
	// for (int i = 0; i < 3; i++)
	{
		testOutput(&ptr[i]);
	}

	free(ptr);

	printf("root directory successfully initiated.\n\n");

	return startingBlock; //returning the location of the directory in LBA
}

void testOutput(dirEntry *rootDir)
{
	//print out its contents
	printf("\n\n---- Directory Contents ----\n");
	printf("Name: %s\n", rootDir->name);
	printf("Location of Self: %ld\n", rootDir->locationLBA);
	printf("Data Location: %ld\n", rootDir->dataLocation);
	printf("EOF LBA:  %ld\n", rootDir->eofLBA);
	printf("EOF Offset (bytes): %d\n", rootDir->eofOffset);
	printf("Child Location: %ld\n", rootDir->childLBA);
	printf("Index: %d\n", rootDir->entryIndex);
	printf("Size of File: %ld\n", rootDir->sizeOfFile);
	printf("Number of Blocks: %ld\n", rootDir->numBlocks);

	struct tm *timeInfo = localtime(&(rootDir->dateCreated));
	printf("DateCreated: %s", asctime(timeInfo));
	timeInfo = localtime(&(rootDir->dateModified));
	printf("DateModified: %s", asctime(timeInfo));
	timeInfo = localtime(&(rootDir->dateAccessed));
	printf("DateAccessed: %s", asctime(timeInfo));

	printf("Location of Metadata: %ld\n", rootDir->locationMetadata);
	printf("isBeingUsed: %s\n", rootDir->isBeingUsed ? "true" : "false");
	//printf("type: %s\n\n", rootDir->type);
}

/*int main() 
{
	printf("rootDirInit executed without errors: %s\n\n", initDirectory(0) > 0 ? "true" : "false");
}*/