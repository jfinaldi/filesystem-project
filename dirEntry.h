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
	unsigned long rootDir;		// location of root directory LBA
	unsigned long locationSelf; //location of this entry, logical block
	//unsigned long id; // id of the file
	unsigned long dataLocation; // location where file data starts
	char name[200];				// directory name for self
	char root[200];				// directory name for root
	//short permission[3]; // three-digit code for rwx, owner/group/all, 0-7
	//unsigned int owner; // the user ID of the owner of the file
	uint64_t sizeOfFile;	 // the number of bytes of the file data
	unsigned long numBlocks; // the number of blocks occupied by the file
	//unsigned long parent;   // # of block where parent directory is located
	time_t dateModifiedDirectory;	// date the file was last modified
	time_t dateAccessedDirectory;	// date the file was last accessed
	unsigned long locationMetadata; //512 file per directory
	unsigned short isBeingUsed;		//tells whether this entry is currently in use or not
} dirEntry;

void initEntry(dirEntry *dE);

#endif