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

#define TRUE 1
#define FALSE 0

#define EXIT_FAILURE 1
#define MAX_OPENFILE 20

#define B_CHUNK_SIZE 512

#define MAXFCBS 20
#define BUFSIZE 512

int b_open(char *filename, int flags);
int b_read(int fd, char *buffer, int count);
int b_write(int fd, char *buffer, int count);
int b_seek(int fd, off_t offset, int whence);
void b_close(int fd);

#endif
