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

#include "dirEntry.h"

//#define NUM_BLOCKS 5  //how many blocks we will allocate by default
//#define BLOCK_SIZE 512 //size of one LBA block

void initEntry(dirEntry* dE) {
	dE->rootDir = 0; // location of first root directory  
	//dE->id = 0; // id of the file
	dE->dataLocation = 2020; // location where file data starts 
	dE->locationSelf = 2020; //location of this entry in logical block

	//initialize a default name
	dE->name[0] = '%';
	dE->name[1] = '\0';
	dE->root[0] = '%';
	dE->root[1] = '\0';

	dE->sizeOfFile = 0;   // the number of bytes of the file data
	dE->numBlocks = 0; // the number of blocks occupied by the file
	
	time(&(dE->dateModifiedDirectory));  // date the file was last modified
	time(&(dE->dateAccessedDirectory));  // date the file was last accessed
	
	dE->locationMetadata = 2020; //512 file per directory
	dE->isBeingUsed = 0; //this file is currently not being used
}
