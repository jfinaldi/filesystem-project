/**************************************************************
* Class:  CSC-415-0# 
* Name:
* Student ID:
* Project: File System Project
*
* File: MakeRemove.h
*
* Description: Interface with shell fonction
*
**************************************************************/

#ifndef _MAKEREMOVE_H
#define _MAKEREMOVE_H

#include "mfs.h"

int fs_mkdir(const char *pathname, mode_t mode);
int fs_rmdir(const char *pathname);
int fs_remove_helper(dirEntry *deToRemove);
char *fs_getcwd(char *buf, size_t size);
int fs_setcwd(char *buf);
char parsePath(char *buf);

#endif