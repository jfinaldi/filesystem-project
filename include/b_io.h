/**************************************************************
* Class:  CSC-415-02 
* Name: Lilian Gouzeot/Jennifer Finaldi(DreamTeam)
* Student ID:
* Project: File System Project
*
* File: b_io.h
*
* Description: Header for readwrite.c struct definition for
*           a structure that stores information about an
*           open file.
*
**************************************************************/

#ifndef _B_IO_H
#define _B_IO_H

#include "mfs.h"

typedef int b_io_fd;

typedef struct fd_struct
{
    int Fd;
    int isAllocate;
    int flag; //stores read/write permissions
    _Bool flaggedForClose; //true when file is about to close

    //dirEntry variable clones
    unsigned long locationLBA;
    unsigned long childLBA;
    short entryIndex;
    unsigned long dataLocation;
    char name[256];
    uint64_t sizeOfFile;
    unsigned long numBlocks;
    time_t dateCreated;
    time_t dateModified;
    time_t dateAccessed;
    unsigned long locationMetadata;
    unsigned long extents;
    unsigned short numExtents;
    unsigned short numExtentBlocks;
    unsigned char type;


    //buffer and tracking variables
    unsigned long filePointer; //file pointer tracks the current byte in the file
    unsigned long blockPointer;
    char *buffer;              // For b_read 
    char *writeBuffer;         // for b_write
    short bufIndex;            //tracks where in the buffer we are. used in b_read
    short buflen;              //tracks how much of buffer is being used, used in b_read
    int lenBuffer;             //used in b_write
    unsigned long LBAInDataLocation; //LBA block in file blob. Will need to be adjusted
                                     //to utilize extents
    short offsetInDataLocation;//tracks bytes read in an LBA block
    int extentArrayPtrRead;    //tracks the logical index in extents array for read
    int extentArrayPtrWrite;   //tracks the logical index in extents array for write
} fd_struct;
extern fd_struct *fileOpen;

int b_open(char *filename, int flags);
int b_read(int fd, char *buffer, int count);
int b_write(int fd, char *buffer, int count);
int b_seek(int fd, off_t offset, int whence);
void b_close(int fd);

#endif
