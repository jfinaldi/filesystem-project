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

void initEntry(dirEntry *dE)
{
	//initialize location variables
	dE->dataLocation = 20000; //valid data location will be between block 0-19531
	dE->locationLBA = 2020; //location of this entry in logical block
	dE->entryIndex = -1; //the position of this entry in the array of entries

	//initialize a default name
	dE->name[0] = '%';
	dE->name[1] = '\0';

	dE->sizeOfFile = 0; // the number of bytes of the file data
	dE->numBlocks = 0;	// the number of blocks occupied by the file
	//dE->id = -1; //the id number for the entry

	time(&(dE->dateModifiedDirectory)); // date the file was last modified
	time(&(dE->dateAccessedDirectory)); // date the file was last accessed

	dE->locationMetadata = 2020; //512 file per directory
	dE->isBeingUsed = 0;		 //this file is currently not being used
	dE->isFile = 0; //initially this will be a directory until datalocation is != 20000
}
