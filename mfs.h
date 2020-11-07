/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: mfs.h
*
* Description: 
*	This is the file system interface.
*	This is the interface needed by the driver to interact with
*	your filesystem.
*
**************************************************************/

#ifndef _MFS_H
#define _MFS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FT_REGFILE DT_REG
#define FT_DIRECTORY DT_DIR
#define FT_LINK DT_LNK

#ifndef uint64_t
typedef u_int64_t uint64_t;
#endif
#ifndef uint32_t
typedef u_int32_t uint32_t;
#endif

#include "b_io.h"
#include "dirEntry.h"
#include "initDirectory.h"
#include "bitMap.h"
#include "MakeRemove.h"

#define BLOCK_SIZE 512

typedef struct MBRstruct
{
	int init;
	int long volumeSize;
	int long blockSize;
	int long totalBlockLBA;
	char fsType[12];
	char magicNumber[2][8];
	int long freeSpacePos;
	int long rootDirectoryPos;
	unsigned long idCounter; //this keeps track of all of the IDs assigned to dir entries
} MBRstruct;
extern MBRstruct *MBR_st;

struct fs_diriteminfo
{
	unsigned short d_reclen; /* length of this record */
	unsigned char fileType;
	char d_name[256]; /* filename max filename is 255 characters */
};

typedef struct
{
	/*****TO DO:  Fill in this structure with what your open/read directory needs  *****/
	unsigned short d_reclen;		 /*length of this record */
	unsigned short dirEntryPosition; /*which directory entry position, like file pos */
	uint64_t directoryStartLocation; /*Starting LBA of directory */
	char * cwd_path; 
} fdDir;
extern fdDir *fdDirCWD;

struct fs_stat
{
	off_t st_size;		  /* total size, in bytes */
	blksize_t st_blksize; /* blocksize for file system I/O */
	blkcnt_t st_blocks;	  /* number of 512B blocks allocated */
	time_t st_accesstime; /* time of last access */
	time_t st_modtime;	  /* time of last modification */
	time_t st_createtime; /* time of last status change */

	/* add additional attributes here for your file system */
};


int fs_mkdir(const char *pathname, mode_t mode);
int fs_rmdir(const char *pathname);
fdDir *fs_opendir(const char *name);
struct fs_diriteminfo *fs_readdir(fdDir *dirp);
int fs_closedir(fdDir *dirp);

char *fs_getcwd(char *buf, size_t size);
int fs_setcwd(char *buf);	   //linux chdir
int fs_isFile(char *path);	   //return 1 if file, 0 otherwise
int fs_isDir(char *path);	   //return 1 if directory, 0 otherwise
int fs_delete(char *filename); //removes a file
int fs_stat(const char *path, struct fs_stat *buf);

int startPartitionSystem(char *filename, uint64_t *volSize, uint64_t *blockSize);
int closePartitionSystem();
uint64_t LBAwrite(void *buffer, uint64_t lbaCount, uint64_t lbaPosition);
uint64_t LBAread(void *buffer, uint64_t lbaCount, uint64_t lbaPosition);
int initializePartition(int fd, uint64_t volSize, uint64_t blockSize);
int my_getnbr(char const *str);
int MBRinit(uint64_t volumeSize, uint64_t blockSize, char **argv);
char *inttostr(int value, char *string, int base);
void testOutput(dirEntry *rootDir);

#endif
