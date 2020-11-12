/**************************************************************
* Class:  CSC-415-0# 
* Name:
* Student ID:
* Project: File System Project
*
* File: MakeRemove.c
*
* Description: Interface of shellfonction
*
**************************************************************/

#include "mfs.h"

fdDir *tempDirectory(const char *path, int needLast)
{
    printf("\n\n----------------\nin tempDir, \n");
    char temp[256];
    strncpy(temp, fdDirCWD->cwd_path, sizeof(fdDirCWD->cwd_path));
    // char first[0];
    // strncpy(first, path, 0);
    // printf ("FIRST CHAR: %s ahh", &path[0]);
    if (strncmp(&path[0], "/", 1) == 0)
    {
        printf("ABSOLUTE PATH\n");
        strncpy(temp, path, sizeof(path));
    }
    else
    {
        strcat(temp, path);
    }

    int curr = MBR_st->rootDirectoryPos;
    int last = curr;
    fdDir *ans = (fdDir *)malloc(sizeof(fdDir));
    if (strcmp(path, "/") == 0)
    {
        printf("\nis Root\n");
        ans->directoryStartLocation = MBR_st->rootDirectoryPos;
        return ans;
    }
    char *token = strtok(temp, "/");
    int counter = 0;
    int blocks = MBR_st->dirNumBlocks;
    // loop through the string to extract all other tokens
    int foundCount = 0; 
    while (token != NULL)
    {
        printf("TOKEN! %s\n", token); //printing each token
        printf("in loop: \n");
        dirEntry *entryBuffer = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
        LBAread(entryBuffer, blocks, curr);
        int flag = -1;
        for (int i = 0; i < STARTING_NUM_DIR; i++)
        {
            if (strcmp(entryBuffer[i].name, token) == 0)
            {
                printf("HI HELLO %d  < i %ld\n", i, entryBuffer[i].childLBA);
                last = curr;
                curr = entryBuffer[i].childLBA;
                flag = 0;
                break;
            }
        }
        if (flag == -1)
        {
            foundCount ++; 
        }

        token = strtok(NULL, "/");
        if (foundCount > 1) {
            printf ("NOT FOUND RETURNING NULL"); 
            ans-> directoryStartLocation = 20000;
            
            return ans; 
        }
        if (token == NULL)
        {
            if (needLast == 1)
            {
                ans->directoryStartLocation = last;
            }
            else
            {
                ans->directoryStartLocation = curr;
            }
        }
    }
    printf("-------------");
    return ans;
}

fdDir *fs_opendir(const char *name)
{
    //we should know the global variable for fdDir object
    fdDir *result = (fdDir *)malloc(sizeof(fdDir));

    //create a dirEntry pointer, malloc dirBUfMallocSize bytes
    dirEntry *ptr = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
    if (ptr == NULL)
    {
        printf("Error with malloc ln96.\n");
        return ((fdDir *)-1);
    }
    else
        printf("Malloc succeeded\n\n");

    printf("name: %s\n", name);
    fdDir *temp = (fdDir *)malloc(sizeof(fdDir));
    temp = tempDirectory((char *const)name, 0);
    if (temp -> directoryStartLocation == 20000) {
            printf("not a valid path or name"); 
            return NULL;
        }
    temp->streamCount = 0;
    printf("temp->directoryStartLocation: %ld\n", temp->directoryStartLocation);
    return temp;
    //do LBA read to populate the buffer from our fdDirCWD->directoryStartLocation
    // LBAread(ptr, MBR_st->dirNumBlocks, temp->directoryStartLocation);

    // //start a for-loop to iterate through fdDir, sizeof(dirEntry) bytes at a time
    // for (int i = 0; i < MBR_st->dirNumBlocks; i++)
    // {
    //     //if name of entry matches argument name
    //     if (strcmp(ptr[i].name, name) == 0)
    //     {
    //         //fdDirCWD starting directory location should equal child location in ptr
    //         result->directoryStartLocation = ptr[i].childLBA;

    //         //assign directory entry position to index in ptr
    //         result->dirEntryPosition = ptr[i].entryIndex;

    //         //set the path string
    //         char *temp = "/";
    //         strcat(temp, name); //should now be /name
    //         strcat(result->cwd_path, temp);

    //         outputFdDirCWD(result);

    //         free(ptr);
    //         return result;
    //     }
    // }
    // printf("not found.\n");
    // free(ptr);
    // return result;
}

struct fs_diriteminfo *fs_readdir(fdDir *dirp)
{
    printf("in read------\n");
    //malloc an fs_diriteminfo object
    struct fs_diriteminfo *result = (struct fs_diriteminfo *)malloc(sizeof(struct fs_diriteminfo));
    if (result == NULL)
    {
        printf("Error with malloc.\n");
        return ((struct fs_diriteminfo *)-1);
    }
    //else
    //printf("\nMalloc succeeded\n");

    //malloc a dirEntry pointer buffer
    dirEntry *ptr = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
    if (ptr == NULL)
    {
        printf("Error with malloc.\n");
        return ((struct fs_diriteminfo *)-1);
    }
    //else
        //printf("\nMalloc succeeded\n");

    LBAread(ptr, MBR_st->dirNumBlocks, dirp->directoryStartLocation);

    //populate it with information from dirp
    while (ptr[dirp->streamCount].isBeingUsed == 0 && dirp->streamCount < 50)
    {
        dirp->streamCount++;
    }
    if (ptr[dirp->streamCount].isBeingUsed == 1)
    {
        result->fileType = ptr[dirp->streamCount].type;
        strcpy(result->d_name, ptr[dirp->streamCount].name);
        dirp->streamCount++;
        return result;
    }
    else
    {
        return NULL;
    }
}

//this function will free all memory for dirp
int fs_closedir(fdDir *dirp)
{
    //free all memory
    free(dirp);
    dirp = NULL;
    if (dirp)
    {
        printf("Error MakeRemove.c, failed attempt to free dirp, ln 196\n");
        return 1;
    }
    return 0;
}

int fs_mkdir(const char *pathname, mode_t mode)
{
    char *pathWithoutName = malloc(sizeof(pathname));
    char *newName = malloc(256);
    int slash = '/';
    newName = strrchr(pathname, slash);
    if (newName == NULL)
    {
        newName = malloc(256);
        strcpy(newName, pathname);
    }
    else
    {
        newName++;
    }

    printf("new name %s\n", newName);
    fdDir *temp = tempDirectory(pathname, 0);
    if (temp -> directoryStartLocation == 20000) {
            printf("not a valid path or name"); 
            return -1;
        }
    printf("HELLLO, dir start loc: %ld\n", temp->directoryStartLocation);
    dirEntry *entryBuffer = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
    int blocks = MBR_st->dirNumBlocks;
    LBAread(entryBuffer, blocks, temp->directoryStartLocation);
    int freeIndex = -1;
    for (int i = 0; i < STARTING_NUM_DIR; i++)
    {
        if (entryBuffer[i].isBeingUsed == 0)
        {
            printf("FREEE INDEX: %d\n", i);
            freeIndex = i;
            break;
        }
    }
    if (freeIndex == -1)
    {
        return -1;
    }
    entryBuffer[freeIndex].isBeingUsed = 1;
    //printf("\nTRY TO STRCOPY: %s", pathname);
    strcpy(entryBuffer[freeIndex].name, newName);
    entryBuffer[freeIndex].name[sizeof(newName)] = '\0';
    entryBuffer[freeIndex].type = atoi("d");
    //entryBuffer[freeIndex].type = "d";

    entryBuffer[freeIndex].childLBA = initDirectory(temp->directoryStartLocation);
    LBAwrite(entryBuffer, blocks, temp->directoryStartLocation);
    printf("STARTINGBLOCK: %ld\n", temp->directoryStartLocation);
    for (int i = 0; i < 6; i++)
    {
        testOutput(&entryBuffer[i]);
    }
    //free(entryBuffer);

    return 0;
}

int fs_remove_helper(dirEntry *deToRemove)
{
    dirEntry *entryBuff = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
    int blocks = MBR_st->dirNumBlocks;
    deToRemove->isBeingUsed = 0;
    strcpy(deToRemove->name, "deleted\n");
    if (deToRemove -> type != 100) {
        free_mem(deToRemove->locationMetadata, 512 * 20);
    }
    printf("CHILDLABA, %ld\n", deToRemove->childLBA);
    LBAread(entryBuff, blocks, deToRemove->childLBA);
    for (int i = 2; i < STARTING_NUM_DIR; i++)
    {
        // if (0) {
        //     entryBuffer[i] . isBeingUsed = 0;
        //     // deletefile();
        //     // free_mem(fileStartLBA, fileLength);
        // } else
        if (entryBuff[i].isBeingUsed == 1)
        {
            printf("why is this printing %d??\n", i);
            fs_remove_helper(&entryBuff[i]);
        }
    }
    LBAwrite(entryBuff, blocks, deToRemove->childLBA);
    free_mem(deToRemove->childLBA, blocks);

    //free(entryBuffer);
}

int fs_rmdir(const char *pathname)
{
    char *newName = malloc(256);
    int slash = '/';
    newName = strrchr(pathname, slash);
    if (newName == NULL)
    {
        newName = malloc(256);
        strcpy(newName, pathname);
    }
    else
    {
        newName++;
    }
    dirEntry *entryBuffer = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
    int blocks = MBR_st->dirNumBlocks;
    fdDir *temp = tempDirectory(pathname, 1);
    if (temp -> directoryStartLocation == 20000) {
            printf("not a valid path or name"); 
            return -1;
        }
    LBAread(entryBuffer, blocks, temp->directoryStartLocation);

    int remove_index = -1;
    printf("TRYING TO DELETE: %s\n", newName);
    for (int i = 0; i < STARTING_NUM_DIR; i++)
    {
        if (strcmp(entryBuffer[i].name, newName) == 0)
        {
            remove_index = i;
            printf("delete index: %d\n", i);
            break;
        }
    }
    fs_remove_helper(&entryBuffer[remove_index]);
    LBAwrite(entryBuffer, blocks, temp->directoryStartLocation);
    //free(entryBuffer);
    for (int i = 0; i < 6; i++)
    {
        testOutput(&entryBuffer[i]);
    }
}

char *fs_getcwd(char *buf, size_t size)
{

    char *path = malloc(256);
    strncpy(path, fdDirCWD->cwd_path, sizeof(fdDirCWD->cwd_path));
    printf("IN GETCWD: %s\n", path);
    return path;
}

int fs_setcwd(char *buf)
{

    char temp[256];
    strncpy(temp, fdDirCWD->cwd_path, sizeof(fdDirCWD->cwd_path));
    printf("SETTT: %s\n", fdDirCWD->cwd_path);
    if (strcmp(&buf[0], "/") == 0)
    {
        strncpy(temp, buf, sizeof(buf));
    }
    else
    {
        strcat(temp, buf);
        printf("SETTT: %s\n", temp);
    }

    int curr = MBR_st->rootDirectoryPos;
    char *token = strtok(temp, "/");
    int counter = 0;
    char ans[20][256];
    int blocks = MBR_st->dirNumBlocks;
    // loop through the string to extract all other tokens
    while (token != NULL)
    {
        printf("TOKEN! %s\n", token); //printing each token
        printf("in loop---\n");
        dirEntry *entryBuffer = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
        LBAread(entryBuffer, blocks, curr);
        int flag = -1;
        for (int i = 0; i < STARTING_NUM_DIR; i++)
        {
            if (strcmp(entryBuffer[i].name, token) == 0)
            {
                printf("HI HELLO %d  < i %ld\n", i, entryBuffer[i].childLBA);
                curr = entryBuffer[i].childLBA;
                flag = 0;
                break;
            }
        }
        if (flag == -1)
        {
            printf("not found\n");
            return -1;
        }

        token = strtok(NULL, "/");
    }

    fdDirCWD->directoryStartLocation = curr;
    if (strcmp(&buf[0], "/") == 0)
    {
        strncpy(fdDirCWD->cwd_path, buf, sizeof(buf));
        strncpy(fdDirCWD->cwd_path, "\0", 1);
    }
    else
    {

        strcat(fdDirCWD->cwd_path, buf);
        strcat(fdDirCWD->cwd_path, "/\0");
    }
    //TODO : modify fdDirCWD->cwd_path to get rid of . and ..
    printf("CURRCWD  %s\n", fdDirCWD->cwd_path);
    return 0;
}

int fs_isDir(char *path)
{
    //return 1;
    char * newName = malloc(256);
    int slash = '/';
    newName = strrchr(path, slash);
    if (newName == NULL){
        newName = malloc(256);
        strcpy(newName, path);
    } else {
        newName++;
    }

    if (strcmp(path, "/") == 0){
        printf("\nis Root\n");
        return 1;
    }
    dirEntry *entryBuffer = (dirEntry *)malloc(MBR_st -> dirBufMallocSize);
    int blocks = MBR_st -> dirNumBlocks;
    fdDir *temp = tempDirectory(path, 1);
    if (temp -> directoryStartLocation == 20000) {
        printf("not a valid path or name"); 
        return -1;
    }
    LBAread(entryBuffer, blocks, temp -> directoryStartLocation);
    for (int i = 0; i < STARTING_NUM_DIR; i++) {
        if (strcmp(entryBuffer[i].name, newName) == 0)
        { 
           if(entryBuffer[i].type == 100) {
               return 1;
           } else {
               return 0;
           }
        }
    }
}

int fs_isFile(char *path)
{
    return !fs_isDir(path);
}
int fs_delete(char *filename) {
    fs_rmdir(filename); 
}

