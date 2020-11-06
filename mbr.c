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
        numberBlock = volumeSize / blockSize;
        MBR_st->totalBlockLBA = numberBlock;
        strcpy(MBR_st->fsType, "DreamTeamFS");
        strcpy(MBR_st->magicNumber[0], "DIR");
        strcpy(MBR_st->magicNumber[1], "FILE");
        MBR_st->freeSpacePos = 1;     //TODO Change that to the real pos of freespace bitmap.
        int rootStartingBlock = memory_map_init(1);
        MBR_st->rootDirectoryPos = rootStartingBlock; //TODO Change that by the real pos of rootDirectory.
        LBAwrite(MBR_st, 1, 0);
        
        // find_free_index(20);
    }
    return (0);
}
