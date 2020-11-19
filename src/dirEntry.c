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

void initEntry(dirEntry *dE)
{
	//initialize location variables
	dE->dataLocation = DEFAULT_LBA; //valid data location will be between block 0-19531
	dE->locationLBA = DEFAULT_LBA;  //location of this entry in logical block
	dE->entryIndex = -1;	  //the position of this entry in the array of entries
	dE->childLBA = DEFAULT_LBA;
	dE->eofLBA = DEFAULT_LBA;
	dE->eofOffset = DEFAULT_SIZE;

	//initialize a default name for this child
	dE->name[0] = '%';
	dE->name[1] = '\0';

	dE->sizeOfFile = DEFAULT_SIZE; // the number of bytes of the file data
	dE->numBlocks = DEFAULT_SIZE;	// the number of blocks occupied by the file
	//dE->id = -1; //the id number for the entry

	time(&(dE->dateCreated));  // date the file was created
	time(&(dE->dateModified)); // date the file was last modified
	time(&(dE->dateAccessed)); // date the file was last accessed

	//initialize extent block numbers in cols, but not the LBAs
	int extentBlocks = EXTENT_STARTING_BLOCKS;
	for(int i = 0; i < EXTENT_ARRAY_ROWS; i++)
	{
		dE->extent[i][0] = DEFAULT_LBA;
		dE->extent[i][1] = extentBlocks;
		extentBlocks *= 2;
	}

	dE->locationMetadata = DEFAULT_LBA; //512 file per directory
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

    time(&(dE->dateModified)); // date the file was last modified
    time(&(dE->dateAccessed)); // date the file was last accessed

	dE->sizeOfFile = (dE->numBlocks * MBR_st->blockSize) + dE->eofLBA;
}

int addAnExtent(dirEntry* dE)
{
	//find the first extent that doesn't have DEFAULT LOCATION in first col
	//get free space, passing in the second col value for this row
	//update extents allocated
	/*
	Questions
	the return type is an int do you want to return the index the start of the extents LBA block, or the number of blocks added to extent
	
	
	*/
	printf("\nIn dirEntry.c In AddAnExtent function line 84 before for loop\n");
	for (int i = 0; i < 4; i++) { //looping through row 0 and colums 0-3 in for loop
		if (dE->extent[0][i] == 20000) {
				dE->extent[0][i] = find_free_index(2*dE->extent[1][i-1]); // uses the second row i index column to multiply the #blocks of previous 
				//extent by 2 so 20,40,80,160
				printf("\n in if statement Number of blocks allocated in extent column:%d, start position of extent blocks in LBA:%d", dE->extent[1][i], dE->extent[0][i] );
		}
	}


	return 0;
}

/*GETTERS IMPLEMENTED HERE*/
//the printf are for debugging purposes they should be commented out once these getters are assumed to be bug free

unsigned long getLocationLBA(dirEntry *dE) {
	printf("\n in getLocationLBA ln 100\n");
	if(dE)
		return dE->locationLBA;
	printf("error: this entry is null. returning %d\n", DEFAULT_LBA);
	return DEFAULT_LBA;
}


unsigned long getChildLBA(dirEntry *dE) {
	printf("\n in getChildLBA ln 106\n");
	if(dE)
		return dE->childLBA;
	printf("error: this entry is null. returning %d\n",DEFAULT_LBA );
	return DEFAULT_LBA;
}

short getEntryIndex(dirEntry *dE) {
	printf("\n in getEntryIndex ln 111\n");
	if(dE)
		return dE->entryIndex;
	printf("error: this entry is null. returning %d\n", DEFAULT_SIZE);
	return DEFAULT_SIZE;
}

unsigned long getDataLocation(dirEntry *dE) {
	printf("\n in getDataLocation ln 116\n");
	if(dE)
		return dE->dataLocation;
	printf("error: this entry is null. returning %d\n", DEFAULT_LBA);
	return DEFAULT_LBA;
}

unsigned long getEofLBA(dirEntry *dE) {
	printf("\n in getEofLBA ln 121\n");
	if(dE)
		return dE->eofLBA;
	printf("error: this entry is null. returning %d\n", DEFAULT_LBA);
	return DEFAULT_LBA;
}

short getEofOffset(dirEntry *dE) {
	printf("\n in getEofOffset ln 126\n");
	if(dE)
		return dE->eofOffset;
	printf("error: this entry is null. returning %d\n", DEFAULT_SIZE);
	return DEFAULT_SIZE;
}

char* getName(dirEntry *dE) {
	printf("\n in getName ln 131 returning pointer to name, this might be buggy\n");
	if(dE)
		return dE->name;
	printf("error: this entry is null. returning NULL\n");
	return NULL;
}

uint64_t getSizeOfFile(dirEntry *dE) {
	printf("\n in getSizeOfFile ln 136\n");
	if(dE)
		return dE->sizeOfFile;
	printf("error: this entry is null. returning %d\n", DEFAULT_SIZE);
	return DEFAULT_SIZE;
}

unsigned long getNumBlocks(dirEntry *dE) {
	printf("\n in getNumBlocks ln 141\n");
	if(dE)
		return dE->numBlocks;
	printf("error: this entry is null. returning %d\n", DEFAULT_SIZE);
	return DEFAULT_SIZE;
}

time_t getDateCreated(dirEntry *dE) {
	printf("\n in getDateCreated ln 146\n");
	if(dE)
		return dE->dateCreated;
	printf("error: this entry is null. returning %d\n", DEFAULT_SIZE);
	return DEFAULT_SIZE;
}

time_t getDateModified(dirEntry *dE) {
	printf("\n in getDateModified ln 151\n");
	if(dE)
		return dE->dateModified;
	printf("error: this entry is null. returning %d\n", DEFAULT_SIZE);
	return DEFAULT_SIZE;
}

time_t getDateAccessed(dirEntry *dE) {
	printf("\n in getDateAccessed ln 156\n");
	if(dE)
		return dE->dateAccessed;
	printf("error: this entry is null. returning %d\n", DEFAULT_SIZE);
	return DEFAULT_SIZE;
}

unsigned long getLocationMetadata(dirEntry *dE) {
	printf("\n in getLocationMetadata ln 161\n");
	if(dE)
		return dE->locationMetadata;
	printf("error: this entry is null. returning %d\n", DEFAULT_LBA);
	return DEFAULT_LBA;
}

unsigned short* getExtent(dirEntry *dE) {
	printf("\n in getExtent ln 166 this one might be buggy\n"); // compiler error ask about this tomorrow
	if(dE)
		return &dE->extent[0][0];
	printf("error: this entry is null. returning NULL\n");
	return NULL;
}

unsigned short getIsBeingUsed(dirEntry *dE) {
	printf("\n in getIsbeingUsed ln 171\n");
	if(dE)
		return dE->isBeingUsed;
	printf("error: this entry is null. returning 3\n");
	return 3;
}

unsigned char getType(dirEntry *dE) {
	printf("\n in getType ln 176\n");
	if(dE) 
		return dE->type;
	printf("error: this entry is null. returning z\n");
	return 'z';
}

/*SETTERS IMPLEMENTED HERE*/
//the printf are for debugging purposes they should be commented out once these getters are assumed to be bug free
//these might be more buggy as they change data


int setLocationLBA(dirEntry *dE, unsigned long newLocationLBA) {
	printf("\n in setLocationLBA ln 184\n");
	if (dE == NULL) {
		printf("\ndE struct pointer is null returning 1\n");
		return 1;
	}
	else {
		printf("\n Successful assigning new LocationLBA\n");
		dE->locationLBA = newLocationLBA;
		return 0;
	}
}

int setChildLBA(dirEntry *dE, unsigned long newChildLBA) {
	printf("\n in setChildLBA ln 199\n");
	if (dE == NULL) {
		printf("\ndE struct pointer is null returning 1\n");
		return 1;
	}
	else {
		printf("\n Successful assigning new LocationLBA\n");
		dE->childLBA = newChildLBA;
		return 0;
	}
}

int setEntryIndex(dirEntry *dE, short newEntryIndex){
	printf("\n in setEntryIndex ln 212\n");
	if (dE == NULL) {
			printf("\ndE struct pointer is null returning 0\n");
			return 0;
		}
	else {
			printf("\n Successful assigning new EntryIndex\n");
			dE->entryIndex = newEntryIndex;
			return 1;
		}
}

int setDataLocation(dirEntry *dE, unsigned long newDataLocation) {
	printf("\n in setDataLocation ln 225\n");
	if(dE == NULL){
		printf("\ndE struct pointer is null returning 1\n");
		return 1;
	}
	else {
		printf("\n Successful assigning new DataLocation\n");
		dE->dataLocation = newDataLocation;
		return 0;
	}
}

int setEofLBA(dirEntry *dE, unsigned long newEofLBA) {
	printf("\n in setEofLBA ln 238\n");
	if (dE == NULL) {
		printf("\ndE struct pointer is null returning 1\n");
		return 1;
	}
	else {
		printf("\n Successful assigning new EofLBA\n");
		dE->eofLBA = newEofLBA;
		return 0;
	}
}

int setEofOffset(dirEntry *dE, short newEofOffset) {
	printf("\n in setEofOffset ln 251\n");
	if (dE == NULL) {
		printf("\ndE struct pointer is null returning 0\n");
		return 0;
	}
	else{
		printf("\n Sucessful assigning new EofOffset\n");
		dE->eofOffset = newEofOffset;
		return 1;
	}
}

int setName(dirEntry *dE, char newName[256]) {
	printf("\n in setName ln 264\n");
	if (dE == NULL) {
		printf("\ndE struct pointer is null returning 1\n");
		return 1;
	}
	else {
		printf("\n Sucessful assigning new Name\n");
		for (int i = 0; i < 256; i++) {
			dE->name[i] = newName[i];
		}
		return 0;
	}
}

int setSizeOfFile(dirEntry *dE, uint64_t newSize) {
	printf("\n in setSizeOfFile ln 277\n");
	if (dE == NULL) {
		printf("\ndE struct pointer is null returning 1\n");
		return 1;
	}
	else {
		printf("\n Sucessful assigning new Size\n");
		dE->sizeOfFile = newSize;
		return 0;
	}
}

int setNumBlocks(dirEntry *dE, unsigned long newNumBlocks) {
	printf("\n in setNumBlocks ln 292\n");
	if (dE == NULL) {
		printf("\ndE struct pointer is null returning 1\n");
		return 1;
	}
	else {
		printf("\n Sucessful assigning new NumBlocks\n");
		dE->numBlocks = newNumBlocks;
		return 0;
	}
}

int setDateCreated(dirEntry *dE, time_t newDateCreated) {
	printf("\n in setDateCreated ln 305 \n");
	if (dE == NULL) {
		printf("\ndE struct pointer is null returning 1\n");
		return 1;
	}
	else {
		printf("\n Sucessful assigning new DateCreated \n");
		dE->dateCreated = newDateCreated;
		return 0;
	}
}

int setDateModified(dirEntry *dE, time_t newDateModified) {
	printf("\n in setDateModified ln 318 \n");
	if (dE == NULL) {
		printf("\ndE struct pointer is null returning 1\n");
		return 1;
	}
	else {
		printf("\n Sucessful assigning new DateModified\n");
		dE->dateModified = newDateModified;
		return 0;
	}
}

int setDateAccessed(dirEntry *dE, time_t newDateAccessed) {
	printf("\n in setDateAccessed ln 331 \n");
	if (dE == NULL) {
		printf("\ndE struct pointer is null returning 1\n");
		return 1;
	}
	else{
		printf("\n Sucessful assigning new DateAccessed \n");
		dE->dateAccessed = newDateAccessed;
		return 0;
	}
}

int setLocationMetadata(dirEntry *dE, unsigned long newLocationMetadata) {
	printf("\n in setLocationMetaData ln 344 \n");
	if (dE == NULL) {
		printf("\ndE struct pointer is null returning 1\n");
		return 1;
	}
	else {
		printf("\n Sucessful assigning new LocationMetadata \n");
		dE->locationMetadata = newLocationMetadata;
		return 0;
	}
}

int setExtent(dirEntry *dE, unsigned short newExtent) {
	return 0; //this one i need to talk about with jenn
	// we might need to allocate more extents
}

int setIsBeingUsed(dirEntry *dE, unsigned short newIsBeingUsed) {
	printf("\n in setIsBeingUsed ln 362 \n");
	if (dE == NULL) {
		printf("\ndE struct pointer is null returning 1\n");
		return 1;
	}
	else {
		printf("\n Sucessful assigning new IsBeingUsed \n");
		dE->isBeingUsed = newIsBeingUsed;
		return 0;
	}
}

int setType(dirEntry *dE, unsigned char newType) {
	printf("\n in setType ln 375 \n");
	if (dE == NULL) {
		printf("\ndE struct pointer is null returning 1\n");
		return 1;
	}
	else {
		printf("\n Sucessful assigning new Type \n");
		dE->type = newType;
		return 0;
	}
}
