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

//#include "dirEntry.h"

#define NUM_BLOCKS 5  //how many blocks we will allocate by default
#define BLOCK_SIZE 512 //size of one LBA block

void initEntry(dirEntry* dE) {
	dE->rootDir = 0; // location of first root directory  
	dE->id = 0; // id of the file
	dE->dataLocation = 0; // location where file data starts 

	//initialize a default name
	dE->name[0] = 'd';
	dE->name[1] = 'e';
	dE->name[2] = 'f';
	dE->name[3] = 'a';
	dE->name[4] = 'u';
	dE->name[5] = 'l';
	dE->name[6] = 't';
	dE->name[7] = '\0';

	dE->permission[0] = 7; // three-digit code for rwx, owner/group/all, 0-7
	dE->permission[1] = 7;
	dE->permission[2] = 7;
	dE->owner = 0; // the user ID of the owner of the file
	dE->sizeOfFile = 0;   // the number of bytes of the file data
	dE->numBlocks = 0; // the number of blocks occupied by the file
	dE->parent = 0;   // # of block where parent directory is located
	time(&(dE->dateModifiedDirectory));  // date the file was last modified
	time(&(dE->dateAccessedDirectory));  // date the file was last accessed
	dE->locationMetadata = 0; //512 file per directory
	dE->isBeingUsed = 0; //this file is currently not being used
}

