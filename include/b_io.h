/**************************************************************
* Class:  CSC-415-0# 
* Name:
* Student ID:
* Project: File System Project
*
* File: b_io.h
*
* Description: Interface of basic I/O functions
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
    unsigned long locationLBA;
    int lenBuffer;
    unsigned long childLBA;
    short entryIndex;
    unsigned long dataLocation;
    unsigned long indexInDataLocation;
    char name[256];
    uint64_t sizeOfFile;
    unsigned long numBlocks;
    time_t dateCreated;
    time_t dateModified;
    time_t dateAccessed;
    unsigned long locationMetadata;
    unsigned short isBeingUsed;
    unsigned char type;
    char *writeBuffer;
    int flag; //stores read/write permissions

    unsigned long filePointer; //file pointer tracks the current byte in the file
    char *buffer;              // For b_read and b_write
    short bufIndex;            //tracks where in the buffer we are
    short buflen;              //tracks how much of buffer is being used

    //Read/write variables
    unsigned long eofLBA;       //last LBA block of our file data
    short eofOffset;            //offset in byes from the start of eofLBA
    short offsetInDataLocation; //tracks bytes read in an LBA block
} fd_struct;
extern fd_struct *fileOpen;

int b_open(char *filename, int flags);
int b_read(int fd, char *buffer, int count);
int b_write(int fd, char *buffer, int count);
int b_seek(int fd, off_t offset, int whence);
void b_close(int fd);

#endif
