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
    char *BufferRead;  // For b_read
    char *BufferWrite; // For b_write

    unsigned long locationLBA;
    unsigned long childLBA;
    short entryIndex;
    unsigned long dataLocation;
    char name[256];
    uint64_t sizeOfFile;
    unsigned long numBlocks;
    time_t dateModifiedDirectory;
    time_t dateAccessedDirectory;
    unsigned long locationMetadata;
    unsigned short isBeingUsed;
    unsigned char type;

    int flag; //stores read/write permissions
    int indexRead; //index in read buffer we are at
    int indexWrite; //index in write buffer we are at
    int buflenRead; //how much of read buffer is occupied
    int buflenWrite; //how much of write buffer is occupied
    unsigned long indexInDataLocation; //place marker, increments 1 for every LBAread call
} fd_struct;
extern fd_struct *fileOpen;

int b_open(char *filename, int flags);
int b_read(int fd, char *buffer, int count);
int b_write(int fd, char *buffer, int count);
int b_seek(int fd, off_t offset, int whence);
void b_close(int fd);

#endif
