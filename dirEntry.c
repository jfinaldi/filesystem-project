/**************************************************************
* Class:  CSC-415 Section #2 
* Name: Jennifer Finaldi (DreamTeam)
* Student ID: 920290420
* Project: File System
*
* File: dirEntry.c
*
* Description: This function implements functions in dirEntry.h
*        Creates and modifies a directory entry
*
**************************************************************/

#include "mfs.h"

#define EXTENT_ARRAY_ROWS 4
#define EXTENT_STARTING_BLOCKS 20
#define DEFAULT_LOCATION 20000

void initEntry(dirEntry *dE)
{
	//initialize location variables
	dE->dataLocation = DEFAULT_LOCATION; //valid data location will be between block 0-19531
	dE->locationLBA = DEFAULT_LOCATION;  //location of this entry in logical block
	dE->entryIndex = -1;	  //the position of this entry in the array of entries
	dE->childLBA = DEFAULT_LOCATION;
	dE->eofLBA = DEFAULT_LOCATION;
	dE->eofOffset = 0;

	//initialize a default name for this child
	dE->name[0] = '%';
	dE->name[1] = '\0';

	dE->sizeOfFile = 0; // the number of bytes of the file data
	dE->numBlocks = 0;	// the number of blocks occupied by the file
	//dE->id = -1; //the id number for the entry

	time(&(dE->dateCreated));  // date the file was created
	time(&(dE->dateModified)); // date the file was last modified
	time(&(dE->dateAccessed)); // date the file was last accessed

	//initialize extent block numbers in cols, but not the LBAs
	int extentBlocks = EXTENT_STARTING_BLOCKS;
	for(int i = 0; i < EXTENT_ARRAY_ROWS; i++)
	{
		dE->extent[i][0] = DEFAULT_LOCATION;
		dE->extent[i][1] = extentBlocks;
		extentBlocks *= 2;
	}

	dE->locationMetadata = DEFAULT_LOCATION; //512 file per directory
	dE->isBeingUsed = 0;		  //this file is currently not being used
	dE->type = atoi("d");				  //initially this will be a directory until datalocation is != 20000
}

/*
This function takes an fd_struct object, and a directory entry and updates
certain information pertaining to the modification of file data.
*/
void updateEntry(fd_struct* fd, dirEntry* dE)
{
    dE->numBlocks = fd->numBlocks;
    dE->eofLBA = fd->eofLBA;
    dE->eofOffset = fd->eofOffset;

    time(&(dE->dateModifiedDirectory)); // date the file was last modified
    time(&(dE->dateAccessedDirectory)); // date the file was last accessed

	dE->sizeOfFile = (dE->numBlocks * MBR_st->blockSize) + dE->eofLBA;
}

int addAnExtent(dirEntry* dE)
{
	//find the first extent that doesn't have DEFAULT LOCATION in first col
	//get free space, passing in the second col value for this row
	//update extents allocated
	return 0;
}
