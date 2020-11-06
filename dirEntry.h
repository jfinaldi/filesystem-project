/**************************************************************
* Class:  CSC-415 Section #2 
* Name: Jennifer Finaldi (DreamTeam)
* Student ID: 920290420
* Project: File System
*
* File: dirEntry.h
*
* Description: Header file for a directory entry structure
*
**************************************************************/

#ifndef _DIRENTRY_H
#define _DIRENTRY_H

#include "mfs.h"

typedef struct DirectoryEntry
{
	unsigned long locationLBA; //location of this directory, logical block
	unsigned long entryIndex; //the index in the directory array
	//unsigned long id; // id of the file
	unsigned long dataLocation; // location where file data starts
	char name[200];				// directory name for self
	char childName[200];  // directory name for the child
	uint64_t sizeOfFile;	 // the number of bytes of the file data
	unsigned long numBlocks; // the number of blocks occupied by the file
	time_t dateModifiedDirectory;	// date the file was last modified
	time_t dateAccessedDirectory;	// date the file was last accessed
	unsigned long locationMetadata; //512 file per directory
	unsigned short isBeingUsed;		//tells whether this entry is currently in use or not
	unsigned short isFile; //flags if an entry is for a file. 0 means it is a directory
} dirEntry;

void initEntry(dirEntry *dE);

#endif