/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: mbr.c
*
* Description: mbr.c
*
**************************************************************/

#include "mfs.h"

int MBRinit(uint64_t volumeSize, uint64_t blockSize, char **argv)
{
    int MBR = 0;
    int numberBlock = 0;

    LBAread(MBR_st, 1, 0);
    
    if (strcmp(MBR_st->fsType, "DreamTeamFS") == 0)
    {
        printf("The Disk Is Already Format to a DreamTeamFS (The best file system of all time)\n");
    }
    else
    {
        MBR_st->init = 1;
        MBR_st->volumeSize = my_getnbr(argv[2]);
        MBR_st->blockSize = blockSize;
        MBR_st->idCounter = 100000; //this will be the very first ID for an entry struct
        numberBlock = volumeSize / blockSize;
        MBR_st->totalBlockLBA = numberBlock;
        strcpy(MBR_st->fsType, "DreamTeamFS");
        strcpy(MBR_st->magicNumber[0], "DIR");
        strcpy(MBR_st->magicNumber[1], "FILE");
        MBR_st->freeSpacePos = 1;     
        int rootStartingBlock = memory_map_init(1);
        MBR_st->rootDirectoryPos = rootStartingBlock;
        printf("mbr root %d", MBR_st->rootDirectoryPos ) ;
        
        //calculate the number of blocks
	    long structSize = sizeof(dirEntry);
	    long bytes = structSize * STARTING_NUM_DIR;
	    MBR_st->dirNumBlocks = 1 + (bytes / BLOCK_SIZE);
        MBR_st->dirBufMallocSize = blockSize * MBR_st->dirNumBlocks;

        int locationRootDir = initDirectory(0); //initialize the root directory
        
        LBAwrite(MBR_st, 1, 0);
    }
    // fdDirCWD -> cwd_path = "/"; 
    // fdDirCWD -> rec_length = 0; 
    // fdDirCWD -> directoryStartLocation = MBR_st -> rootDirectoryPos; 
    // fdDirCWD -> directoryStartLocation = 0; 
    return (0);
}
