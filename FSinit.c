/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: FSinit.c
*
* Description: FSinit.c
*
**************************************************************/

#include "mfs.h"

int MBRinit(uint64_t volumeSize, uint64_t blockSize, char **argv)
{
    char *filename;
    int MBR = 0;

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
        char *buf = malloc(blockSize);
        memset(buf, 0, blockSize);
        strcpy(buf, "I");     //INIT OR NOT
        strcat(buf, "|");     // | USE TO SEPARATE THE VARIABLE
        strcat(buf, argv[2]); //VOLUME SIZE
        strcat(buf, "|");
        strcat(buf, "512"); // BLOCK SIZE
        strcat(buf, "|");
        strcat(buf, "DreamTeamFS"); // NAME OF THE FS
        strcat(buf, "|");
        strcat(buf, "TREE"); // DIRECTORY STRUCTURE
        strcat(buf, "|");
        strcat(buf, "1"); //FIRST BLOCK OF THE VOLUME
        strcat(buf, "|");
        strcat(buf, "1"); //POINTER TO ROOT DIRECTORY
        strcat(buf, "#"); // # MEAN END OF THE BUFFER
        // printf("buf =%s\n", buf);
        LBAwrite(buf, 1, 0);
        free(buf2);
        free(buf);
    }
}