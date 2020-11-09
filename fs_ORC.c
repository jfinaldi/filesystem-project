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

void outputFdDirCWD();

fdDir *fs_opendir(const char *name)
{
    //we should know the global variable for fdDir object
    fdDir* result = (fdDir*)malloc(sizeof(fdDir));

    //create a dirEntry pointer, malloc dirBUfMallocSize bytes 
    dirEntry* ptr = (dirEntry*)malloc(MBR_st->dirBufMallocSize);
    if (ptr == NULL)
	{
		printf("Error with malloc ln50.\n");
		return -1;
	}
	else
		printf("Malloc succeeded\n\n");
    
    //do LBA read to populate the buffer from our fdDirCWD->directoryStartLocation
    LBAread(ptr, MBR_st->dirNumBlocks, fdDirCWD->directoryStartLocation);

    //start a for-loop to iterate through fdDir, sizeof(dirEntry) bytes at a time
    for(int i = 0; i < MBR_st->dirNumBlocks; i++)
    {
        //if name of entry matches argument name
        if(strcmp(ptr[i].name, name) == 0)
        {
            //fdDirCWD starting directory location should equal child location in ptr
            result->directoryStartLocation = ptr[i].childLBA;
            
            //assign directory entry position to index in ptr
            result->dirEntryPosition = ptr[i].entryIndex;

            //set the path string
            char* temp = "/";
            strcat(temp, name); //should now be /name
            strcat(result->cwd_path, temp);

            outputFdDirCWD(result);

            free(ptr);
            return result;
        }
    }
    printf("%s not found.\n");
    free(ptr);
    return result;
}

struct fs_diriteminfo *fs_readdir(fdDir *dirp)
{
    //malloc an fs_diriteminfo object
    struct fs_diriteminfo* result = (struct fs_diriteminfo*)malloc(sizeof(struct fs_diriteminfo));
    if (result == NULL)
	{
		printf("Error with malloc.\n");
		return -1;
	}
	else
		printf("Malloc succeeded\n\n");

    //malloc a dirEntry pointer buffer
    dirEntry* ptr = (dirEntry*)malloc(MBR_st->dirBufMallocSize);
    if (ptr == NULL)
	{
		printf("Error with malloc.\n");
		return -1;
	}
	else
		printf("Malloc succeeded\n\n");

    LBAread(ptr, MBR_st->dirNumBlocks, dirp->directoryStartLocation);
    
    //populate it with information from dirp
    result->fileType = ptr[dirp->dirEntryPosition].type;
    strcpy(result->d_name, ptr[dirp->dirEntryPosition].name);
    
    //return
    free(ptr);
    return result;
}

//this function will free all memory for dirp
int fs_closedir(fdDir *dirp)
{
    //free all memory
    free(dirp);
    if(dirp)
    {
        printf("Error fsORC.c, failed attempt to free dirp, ln 103\n");
        return 1;
    }
    return 0;
}

void outputFdDirCWD(fdDir* dirp)
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