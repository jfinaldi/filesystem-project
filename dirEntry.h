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
	unsigned long locationLBA; //location of this directory entry, logical block
	unsigned long childLBA;	   //location of the directory this entry points to
	short entryIndex;		   //the index in the directory array
	//unsigned long id; // id of the file
	unsigned long dataLocation;		// location where file data starts
	unsigned long eofLBA;    		// LBA of last block of file data
	short eofOffset;   				// the offset in the LBA block containing EOF
	char name[256];					// directory name for the directory it points to
	uint64_t sizeOfFile;			// the number of bytes of the file data
	unsigned long numBlocks;		// the number of blocks occupied by the file
	time_t dateModifiedDirectory;	// date the file was last modified
	time_t dateAccessedDirectory;	// date the file was last accessed
	unsigned long locationMetadata; //512 file per directory
	unsigned short isBeingUsed;		//tells whether this entry is currently in use or not
	unsigned char type;				//f for file, d for directory
} dirEntry;

void initEntry(dirEntry *dE);
void updateEntry(fd_struct* fd, dirEntry* dE);

#endif