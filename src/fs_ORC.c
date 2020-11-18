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

void outputFdDirCWD(fdDir *dirp)
{
    printf("\n\n-----------------------------------------------\n");
    printf("Printing directory\n");
    printf("-----------------------------------------------");
    printf("Current Working Directory: %s\n", dirp->cwd_path);
    printf("Start Location: %ld\n", dirp->directoryStartLocation);
    printf("Directory Entry Position: %d\n", dirp->dirEntryPosition);
}

/*int main() {
    //int dirLocation = initDirectory(5); //create a fake directory
    char* newDirName = "foo"; //create a name for a new directory
    //dirEntry* ptr; //create a directory pointer 
}*/