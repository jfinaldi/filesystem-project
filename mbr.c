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
    char *filename;
    int MBR = 0;
    int number_of_blocks = 0;

    char *buf2 = malloc(blockSize);
    if (buf2 == NULL)
        return (1);

    LBAread(buf2, 1, 0);

    // printf("\n%c\n", buf2[0]);
    if (buf2[0] == 'I')
    {
        write(1, "MBR already init\n", 17);
        MBR = 1;
        free(buf2);
    }

    if (MBR == 0) //to change to 0
    {
        char snum[512];
        char *buf = malloc(blockSize);
        if (buf == NULL)
            return (1);
        number_of_blocks = volumeSize / blockSize;
        inttostr(number_of_blocks, snum, 10);

        int root_dir_block = memory_map_init(volumeSize, blockSize, 1);
        printf("okay%d", root_dir_block);  

        memset(buf, 0, blockSize);
        strcpy(buf, "I");     //INIT OR NOT
        strcat(buf, "|");     // | USE TO SEPARATE THE VARIABLE
        strcat(buf, argv[2]); //VOLUME SIZE
        strcat(buf, "|");
        strcat(buf, "512"); // BLOCK SIZE
        strcat(buf, "|");
        strcat(buf, snum); // NUMBER OF BLOCK INSIDE LBA
        strcat(buf, "|");
        strcat(buf, "DreamTeamFS"); // NAME OF THE FS
        strcat(buf, "|");
        strcat(buf, "TREE"); // DIRECTORY STRUCTURE
        strcat(buf, "|");
        strcat(buf, "1"); //FIRST BLOCK OF THE VOLUME
        strcat(buf, "|");
        strcat(buf, root_dir_block); //POINTER TO ROOT DIRECTORY
        strcat(buf, "|");
        strcat(buf, "1"); // POINTER TO BITMAP FREESPACE
        strcat(buf, "|");
        strcat(buf, "DIR"); // MAGIC NUMBER FOR A DIR
        strcat(buf, "|");
        strcat(buf, "FILE"); // MAGIC NUMBER FOR A FILE
        strcat(buf, "#");    // # MEAN END OF THE BUFFER
        // printf("buf =%s\n", buf);
        LBAwrite(buf, 1, 0);
        find_free_index(2, volumeSize, blockSize); 
        free(buf2);
        free(buf);
    }
}
