/**************************************************************
* Class:  CSC-415 Section #2 
* Name: Jennifer Finaldi (DreamTeam)
* Student ID: 920290420
* Project: File System
*
* File: dirEntry.h
*
* Description: Header file for a directory entry structure
*			including prototypes for all directory related
*			functions.
*
**************************************************************/

#ifndef _DIRENTRY_H
#define _DIRENTRY_H

#include "mfs.h"

typedef struct DirectoryEntry
{
	unsigned long locationLBA;      //location of this directory entry, logical block
	unsigned long childLBA;	        //location of the directory this entry points to
	short entryIndex;		        //the index in the directory array
	//unsigned long id;             // id of the file
	unsigned long dataLocation;		// location where file data starts
	char name[256];					// directory name for the directory it points to
	uint64_t sizeOfFile;			// the number of bytes of the file data
	unsigned long numBlocks;		// the number of blocks occupied by the file
	time_t dateCreated;			    // the date the file was created
	time_t dateModified;			// date the file was last modified
	time_t dateAccessed;			// date the file was last accessed
	unsigned long locationMetadata; // 512 file per directory
	unsigned long extents;          // an LBA location that stores array of 64 lBAs and sizes
	unsigned short numExtents;      // tracks how many extents currently allocated
	unsigned short numExtentBlocks; // the number of blocks reserved in all extents
	unsigned short isBeingUsed;		// tells whether this entry is currently in use or not
	unsigned char type;				// f for file, d for directory
} dirEntry;

void initEntry(dirEntry *dE);
int updateEntry(int fd, dirEntry* dE);
unsigned long initExtents(dirEntry* dE);
unsigned long addAnExtent(dirEntry* dE);
int returnWastedExtents(dirEntry* dE);
int wipeExtents(dirEntry* dE); 

/*GETTERS*/
unsigned long getLocationLBA(dirEntry *dE);
unsigned long getChildLBA(dirEntry *dE);
short getEntryIndex(dirEntry *dE);
unsigned long getDataLocation(dirEntry *dE);
char* getName(dirEntry *dE); 
uint64_t getSizeOfFile(dirEntry *dE);
unsigned long getNumBlocks(dirEntry *dE);
time_t getDateCreated(dirEntry *dE);
time_t getDateModified(dirEntry *dE);
time_t getDateAccessed(dirEntry *dE);
unsigned long getLocationMetadata(dirEntry *dE);
unsigned short getIsBeingUsed(dirEntry *dE);
unsigned char getType(dirEntry *dE);
unsigned long getExtentLBA(int fd, _Bool isForWrite);
unsigned long getExtents(dirEntry* dE);
unsigned short getNumExtents(dirEntry *dE);
unsigned short getNumExtentBlocks(dirEntry* dE);

/*SETTERS*/
int setLocationLBA(dirEntry *dE, unsigned long newLocationLBA);
int setChildLBA(dirEntry *dE, unsigned long newChildLBA);
int setEntryIndex(dirEntry *dE, short newEntryIndex);
int setDataLocation(dirEntry *dE, unsigned long newDataLocation);
int setName(dirEntry *dE, char newName[256]);
int setSizeOfFile(dirEntry *dE, uint64_t newSize);
int setNumBlocks(dirEntry *dE, unsigned long newNumBlocks);
int setDateCreated(dirEntry *dE, time_t newDateCreated);
int setDateModified(dirEntry *dE, time_t newDateModified);
int setDateAccessed(dirEntry *dE, time_t newDateAccessed);
int setLocationMetadata(dirEntry *dE, unsigned long newLocationMetadata);
int setExtent(dirEntry *dE, unsigned short newExtent);
int setIsBeingUsed(dirEntry *dE, unsigned short newIsBeingUsed);
int setType(dirEntry *dE, unsigned char newType);

#endif