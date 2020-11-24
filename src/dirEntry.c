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
	dE->dataLocation = DEFAULT_LBA; //valid data location will be between block 0-19531
	dE->locationLBA = DEFAULT_LBA;  //location of this entry in logical block
	dE->entryIndex = -1;	  //the position of this entry in the array of entries
	dE->childLBA = DEFAULT_LBA;
	//dE->eofLBA = DEFAULT_LBA;
	//dE->eofOffset = DEFAULT_SIZE;

	//initialize a default name for this child
	dE->name[0] = '%';
	dE->name[1] = '\0';

	dE->sizeOfFile = DEFAULT_SIZE; // the number of bytes of the file data
	dE->numBlocks = DEFAULT_SIZE;	// the number of blocks occupied by the file
	//dE->id = -1; //the id number for the entry

	time(&(dE->dateCreated));  // date the file was created
	time(&(dE->dateModified)); // date the file was last modified
	time(&(dE->dateAccessed)); // date the file was last accessed

	//initialize extent block 
	dE->extents = DEFAULT_LBA;
	dE->numExtents = DEFAULT_SIZE;
	dE->numExtentBlocks = DEFAULT_SIZE;

	dE->locationMetadata = DEFAULT_LBA; //512 file per directory
	dE->isBeingUsed = 0;		  //this file is currently not being used
	dE->type = atoi("d");				  //initially this will be a directory until datalocation is != 20000
}

/*
This function takes an fd_struct object, and a directory entry and updates
certain information pertaining to the modification of file data.
*/
int updateEntry(int fd, dirEntry* dE)
{
	printf("\nupdateEntry.....\n");

	//if we have a valid fd and dE
	if((fd > -1) && dE) {
		//create a buffer for directory with dE in it
		dirEntry* buf = (dirEntry*)malloc(MBR_st->dirBufMallocSize);
		if(!buf){
			printf("Malloc failed ln61. returning 1\n");
			return 1;
		}
		LBAread(buf, MBR_st->dirNumBlocks, dE->locationLBA);

		//copy info from fd to dE
		dE->locationLBA = fileOpen[fd].locationLBA;
		dE->childLBA = fileOpen[fd].childLBA;
		dE->entryIndex = fileOpen[fd].entryIndex;	  //the position of this entry in the array of entries
		dE->dataLocation = fileOpen[fd].dataLocation; //valid data location will be between block 0-19531
		strcpy(dE->name, fileOpen[fd].name);
		dE->sizeOfFile = fileOpen[fd].sizeOfFile;
		dE->numBlocks = fileOpen[fd].numBlocks;

    	time(&(dE->dateModified)); // date the file was last modified
    	time(&(dE->dateAccessed)); // date the file was last accessed

		//if fd is about to be closed
		if(fileOpen[fd].flaggedForClose) {
			if(dE->numBlocks < dE->numExtentBlocks)
				returnWastedExtents(dE); //give back wasted extents
		}
		else { 
			//otherwise, do we need another extent?
			if(dE->numBlocks == dE->numExtentBlocks)
				addAnExtent(dE); //add an extent
		}

		//write all updated entry info to disk
		LBAwrite(buf, MBR_st->dirNumBlocks, dE->locationLBA);

		//free
		if(buf){
			free(buf);
			buf = NULL;
		}

		printf("entry successfully updated\n");
		return 0;
	}
	printf("error: this entry is null. returning 1\n");
	return 1;
}

int returnWastedExtents(dirEntry* dE)
{
	printf("returnWastedExtents...\n");
	//create a buffer for our extents array
	unsigned long* ptr = (unsigned long*)malloc(BLOCK_SIZE);
	if(!ptr) {
		printf("Malloc failed!\n");
		return 1;
	}
	dirEntry* buf = (dirEntry*)malloc(MBR_st->dirBufMallocSize);
	if(!buf) {
		printf("Malloc failed!\n");
		return 1;
	}
	LBAread(ptr, 1, dE->extents); //read the extents blob to buffer
	LBAread(buf, MBR_st->dirNumBlocks, dE->locationLBA); //read a directory to buffer

	unsigned long blocksToReturn = dE->numExtentBlocks - dE->numBlocks;

	//find last element in our extents array blob
	unsigned long lastElem = (dE->numExtentBlocks * 2) - 1;

	//find the actual LBA block start that we're going to free
	unsigned long lbaToReturn = ptr[lastElem - 1];
	lbaToReturn += (ptr[lastElem] - blocksToReturn);
	free_mem(lbaToReturn, blocksToReturn); //free the blocksToReturn
	ptr[lastElem] -= blocksToReturn; //decrement the last element in our extents blob by blocksToReturn
	dE->numExtentBlocks -= blocksToReturn; //decrement numExtentBlocks
	LBAwrite(ptr, 1, dE->extents); //do LBAwrite of file blob to save changes
	LBAwrite(buf, MBR_st->dirNumBlocks, dE->locationLBA); //do LBAwrite of directory to save new entry changes

	if(ptr){
		free(ptr);
		ptr = NULL;
	}
	if(buf) {
		free(buf);
		buf = NULL;
	}
	return 0;
}

// helper function to resolve a logical extent element into an LBA block
unsigned long getExtentLBA(int fd, _Bool isForWrite)
{
	printf("\ngetExtentLBA...\n");

	//get a directory Entry from fd
	dirEntry* buf = (dirEntry*)malloc(MBR_st->dirBufMallocSize);
	LBAread(buf, MBR_st->dirNumBlocks, fileOpen[fd].locationLBA);
	dirEntry* dE = &(buf[fileOpen[fd].entryIndex]);

	//if this is from the very first write, extents needs initializing
	if(isForWrite && (dE->extents == DEFAULT_LBA)) {
		unsigned long result = initExtents(dE);
		printf("initialized first extent to LBA: %ld\n", result);
		free(buf);
		buf = NULL;
		return result;
	}

	//resolve logical index in extents array
	int indexPosition = -1;
	if(isForWrite)
		indexPosition = fileOpen[fd].extentArrayPtrWrite;
	else indexPosition = fileOpen[fd].extentArrayPtrRead;
	if(indexPosition == -1) {
		printf("Error getting correct index position. Returning %d\n", DEFAULT_LBA);
		free(buf);
		buf = NULL;
		return DEFAULT_LBA;
	}

	unsigned long* extentBuffer = (unsigned long*) malloc(512); //creating memory in main space for 512 bytes and partitioning it into chunks of 8 bytes

	if (extentBuffer == NULL) {
		printf("\n cannot allocate memory at ln 98 dirEntry.c\n");
		free(buf);
		buf = NULL;
		return DEFAULT_LBA;
	}


	LBAread(extentBuffer, 1, dE->extents);  //reading the LBA extents block where even indexes = LBA start, and odd = LBA extent size
	
	printf("\n First Extent start comparison with for loop:%ld \n", extentBuffer[0]);
	printf("\n First Extent size comparison with for loop:%ld \n", extentBuffer[1]);

	//change to random number to check if they math like 8, 10, 17, or 25.
	
	for (int bufferIterator = 0; bufferIterator < EXTENT_MAX_ELEMENTS; bufferIterator =+ 2){
		printf("\n In FOR LOOP ln 113 \n");
		printf("\n Extent start location:%ld\n", extentBuffer[bufferIterator]); // test to get the proper locations
		printf("extent Size value:%ld", extentBuffer[bufferIterator + 1]); // test to make sure it is 20,40,80.160, 320.
		
		//check to see if this element has zero in it. If so, it is unused
		if(extentBuffer[bufferIterator] == DEFAULT_SIZE) {
			unsigned long result = DEFAULT_LBA;
			
			//if isForWrite, we will add an extent
			if(isForWrite) {
				result = addAnExtent(dE);
			}
			//otherwise we are just trying to read, so return a dummy number to caller
			else {
				printf("ERROR: end of extents. Returning %ld\n", result);
			}
			//free memory
			if(extentBuffer) {
				free(extentBuffer);
				extentBuffer = NULL;
			}
			if(buf) {
				free(buf);
				buf = NULL;
			}
			return result;
		}
		
		if (indexPosition < extentBuffer[bufferIterator + 1] ) {
			unsigned long finalPosition =  extentBuffer[bufferIterator] + indexPosition;
			printf("\n Final postion:%ld", finalPosition); //confirm this and test it

			if(buf) {
				free(buf);
				buf = NULL;
			}
			if(extentBuffer) {
				free(extentBuffer);
				extentBuffer = NULL;
			}
			return finalPosition; 
		}
		else {
			indexPosition = indexPosition - extentBuffer[bufferIterator + 1];
		}
		
	}

	if(buf) {
		free(buf);
		buf = NULL;
	}
	if(extentBuffer) {
		free(extentBuffer);
		extentBuffer = NULL;
	}

	printf("error getting extent LBA. Returning %d", DEFAULT_LBA);
	return DEFAULT_LBA;
}

unsigned long initExtents(dirEntry* dE)
{
	printf("\n initExtents....\n");
	if(!dE) {
		printf("error, entry is null. Returning 1\n");
		return 1;
	}

	dE->extents = find_free_index(1); //assign one block for the extents array
	unsigned long* ptr = (unsigned long*)malloc(BLOCK_SIZE);

	//initialize all 64 elements to zeros
	for(int i = 0; i < EXTENT_MAX_ELEMENTS; i++)
	{
		ptr[i] = DEFAULT_SIZE;
	}

	//get free space of 20 blocks for the first extent
	unsigned long firstExt = find_free_index(EXTENT_START_BLOCKS);
	printf("firstExt: %ld\n", firstExt);

    //and store the LBA in the first position of buffer
	ptr[0] = firstExt;
	ptr[1] = EXTENT_START_BLOCKS;
    
    //increment numExtents and numBlocks
	dE->numExtents++;
	//dE->numBlocks++;
	dE->numExtentBlocks += EXTENT_START_BLOCKS;
    
    //do an LBAwrite of the buffer, passing in buf, 1, dE->extents
	LBAwrite(ptr, 1, dE->extents);

	unsigned long result = ptr[0];

	if(ptr) {
		free(ptr);
		ptr = NULL;
	}
    
    return result;
}

unsigned long addAnExtent(dirEntry* dE) 
{
    printf("\naddAnExtent.....\n");
    if(!dE) {
        printf("error, entry is null. returning 1\n");
        return 1;
    }
	if(dE->numExtents == (EXTENT_MAX_ELEMENTS / 2)) {
		printf("error, extents are full for some insane reason.\n");
		return 1;
	}
    
    //if extent LBA location hasn't been started, then get one
    if(dE->extents == DEFAULT_LBA) {
        return initExtents(dE);
	}
    
    //create a buffer and read in our extents LBA
    unsigned long* ptr = (unsigned long*)malloc(BLOCK_SIZE);
    LBAread(ptr, 1, dE->extents);
    
    unsigned short lastElement = (dE->numExtents * 2) - 1; // get the first avail element in LBA
    
    //calculate the number of blocks we're giving this extent
    unsigned long prevExtBlocks = ptr[lastElement];
    unsigned long newExtBlocks = prevExtBlocks * EXTENT_MULTIPLIER;
    unsigned long newExtStartIndex = lastElement + 1;
    unsigned long newExtBlocksIndex = lastElement + 2;

	//output stuff
	printf("prevExtBlocks: %ld\n", prevExtBlocks);
	printf("newExtBlocks: %ld\n", newExtBlocks);
	printf("newExtStartIndex: %ld\n", newExtStartIndex);
	printf("newExtBlocksIndex: %ld\n", newExtBlocksIndex);
    
    //get free space for this extent
    unsigned long newBlock = find_free_index(newExtBlocks);

	//check to see if this newBlock is right after the end of the previous extent
	unsigned long prevExtBlock = ptr[lastElement - 1];
	unsigned long lastLBAOfPrevExtent = prevExtBlock + prevExtBlocks;
	if((lastLBAOfPrevExtent + 1) == newBlock)
	{
		//instead, incrementing the previous extent size by newExtBlocks
		ptr[lastElement] += newExtBlocks;
		//do a write
		//free memory
		//return
	}
    else {
    	//write the newBlock number to newExtStartIndex
    	ptr[newExtStartIndex] = newBlock; 		//write the newBlock number to newExtStartIndex
    	ptr[newExtBlocksIndex] = newExtBlocks;  //write the newBlocks number to newExtBlocksIndex
		dE->numExtents++; 						//increment the number of extents by 1
	}

	//output stuff
	printf("newBlock: %ld\n", newBlock);
	printf("ptr[%ld]: %ld\n", newExtStartIndex, ptr[newExtStartIndex]);
	printf("ptr[%ld]: %ld\n", newExtBlocksIndex, ptr[newExtBlocksIndex]);
    
	dE->numExtentBlocks += newExtBlocks; 		//increment the number of extent blocks total

	//output stuff
	printf("dE->numExtents: %d\n", dE->numExtents);
	printf("dE->numBlocks: %ld\n", dE->numBlocks);

    
    //do an LBAwrite, passing in buffer, 1, dE->extents
	LBAwrite(ptr, 1, dE->extents);

	if(ptr) {
		free(ptr);
		ptr = NULL;
	}
    
    return newBlock;
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

/*unsigned long getEofLBA(dirEntry *dE) {
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
}*/

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

unsigned long getExtents(dirEntry *dE) {
	printf("\n in getExtents ln 359\n"); // compiler error ask about this tomorrow
	if(dE)
		return dE->extents;
	printf("error: this entry is null. returning %d\n", DEFAULT_LBA);
	return DEFAULT_LBA;
}

unsigned short getNumExtents(dirEntry *dE) {
	printf("\n in getNumExtents.....\n"); // compiler error ask about this tomorrow
	if(dE)
		return dE->numExtents;
	printf("error: this entry is null. returning %d\n", DEFAULT_SIZE);
	return DEFAULT_SIZE;
}

unsigned short getNumExtentBlocks(dirEntry *dE) {
	printf("\n in getNumExtentBlocks.....\n"); // compiler error ask about this tomorrow
	if(dE)
		return dE->numExtentBlocks;
	printf("error: this entry is null. returning %d\n", DEFAULT_SIZE);
	return DEFAULT_SIZE;
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

/*int setEofLBA(dirEntry *dE, unsigned long newEofLBA) {
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
}*/

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
