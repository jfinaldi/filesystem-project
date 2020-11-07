/**************************************************************
* Class:  CSC-415 Section #2 
* Name: Jennifer Finaldi (DreamTeam)
* Student ID: 920290420
* Project: File System
*
* File: fs_ORC.c
*
* Description: Open, Read, Close functions to interface between
*         directories and the shell
*
**************************************************************/
#include "mfs.h"

fdDir *fs_opendir(const char *name)
{
    //we should know the global variable for fdDir object
    //create a dirEntry pointer, malloc a 
    //dirEntry* ptr = cur_fdDir->;
    //start a for-loop to iterate through fdDir, sizeof(dirEntry) bytes at a time

        //if name of entry matches argument name
            //childLocation should equal the starting directory locaton in fdDir
            //
}

struct fs_diriteminfo *fs_readdir(fdDir *dirp)
{

}

int fs_closedir(fdDir *dirp)
{

}

/*int main() {
    //int dirLocation = initDirectory(5); //create a fake directory
    char* newDirName = "foo"; //create a name for a new directory
    //dirEntry* ptr; //create a directory pointer 
}*/