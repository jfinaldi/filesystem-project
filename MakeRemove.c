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

char** tokenizePath(char* path, int* i)
{
    printf("[367]In tokenizePath...\n");
    char** result = (char**)malloc(strlen(path) * sizeof(char**));

    // loop through the string to extract all other tokens
    //printf("in tokenizing while-loop\n-------------\n");
    char* token = strtok(path, "/");
    //printf("token: %s\n", token);
    (*i) = 0;
    while (token != NULL) 
    {
        printf("i: %d\ntoken: %s\n", *i, token);
        result[*i] = token;
        printf("result[%d]: %s\n", (*i), result[*i]);
        (*i)++;
        token = strtok(NULL, "/");
    } 
    //printf("path: %s\n", path);
    printf("numTokens: %d\n", *i);


    printf("printing tokens:\n");
    for(int j = 0; j < (*i); j++) {
        printf("tokens[%d]: %s\n", j, result[j]);
    }
    return result;
}

fdDir *tempDirectory(const char *path, int needLast)
{
    printf("\n\n----------------\nin tempDir, \n");

    char tempPath[256];

    //copy the current working directory path into tempPath
    strncpy(tempPath, fdDirCWD->cwd_path, sizeof(fdDirCWD->cwd_path));
   
    //if we have an absolute path
    if (path[0] == '/') 
    {
        printf("ABSOLUTE PATH\n");
        strncpy(tempPath, path, sizeof(path)); //copy the path directly in temp
    }
    else //otherwise we have a relative path
    {
        strcat(tempPath, path); //add relative path to temp 
    }

    int curr = MBR_st->rootDirectoryPos; //get root LBA start
    int last = curr; //cache the root LBA start
    fdDir *ans = (fdDir *)malloc(sizeof(fdDir)); //create a pointer to an fdDir struct

    //if our path is literally just a / then we dupe the root dir
    if (strcmp(path, "/") == 0)
    {
        printf("\nis Root\n");
        ans->directoryStartLocation = MBR_st->rootDirectoryPos;
        return ans;
    }

    //char *token = strtok(temp, "/");
    int numTokens = 0;
    char** tokens = tokenizePath(tempPath, &numTokens);
    printf("[50]tokens[0]: %s\n", tokens[0]);
    int counter = 0;
    int blocks = MBR_st->dirNumBlocks;

    // loop through all of the tokens
    int foundCount = 0; 
    for(int i = 0; i < numTokens; i++)
    {
        printf("token[%d]: %s\n", i, tokens[i]); //printing each token
        dirEntry *entryBuffer = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
        printf("ln52\n");
        LBAread(entryBuffer, blocks, curr);
        printf("ln54\n");
        int flag = -1;

        //loop through all of the directory entries
        for (int j = 0; j < STARTING_NUM_DIR; j++)
        {
            printf("%s =? %s\n", entryBuffer[i].name, tokens[i]);
            if (strcmp(entryBuffer[j].name, tokens[i]) == 0)
            {
                printf("%s matches %s at %ld\n", entryBuffer[j].name, tokens[i], entryBuffer[j].childLBA);
                last = curr;
                curr = entryBuffer[j].childLBA;
                flag = 0;
                break; //we found a match, jump to the next token
            }
        }
        printf("I did not find a duplicate name.\n");
        if (flag == -1)
        {
            foundCount++; 
        }

        if (foundCount > 1) {
            printf ("NOT FOUND RETURNING NULL\n"); 
            ans-> directoryStartLocation = 20000;
            
            return ans; 
        }
        if (tokens[i] == NULL)
        {
            if (needLast == 1) //needLast is true
            {
                ans->directoryStartLocation = last; //our start location is the second to last dir entry
            }
            else
            {
                ans->directoryStartLocation = curr; //if we don't need last, then current location is good
            }
        }
        if(entryBuffer) {
            free(entryBuffer);
            entryBuffer = NULL;
        }
        if(entryBuffer) 
        {
            printf("Error deallocating entryBuffer in tempDirectory makeremove.c\n");
        }

        printf("Jenn's token: %s\n", tokens[i]);
    }
    printf("-------------\n");

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
    //deallocate diriteminfo struct
    if(dirp->dirItemInfo)
    {
        free(dirp->dirItemInfo);
        dirp->dirItemInfo = NULL;
    }
    if(dirp->dirItemInfo)
    {
        printf("Error fs_closedir failed to deallocate dirp->dirItemInfo\n");
        return 1;
    }

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
    char* ptr;
    int slash = '/';
    ptr = strrchr(pathname, slash); //find the last slash

    //path name has no slashes at all
    if (ptr == NULL)
        strcpy(newName, pathname);

    //we found a last slash
    else
    {
        ptr++;
        strcpy(newName, ptr);
    }
    printf("new name %s\n", newName);
    fdDir *temp = tempDirectory(pathname, 0); //create a temp fdDir struct
    printf("HELLLO, dir start loc: %ld\n", temp->directoryStartLocation);

    //create a directory buffer and fill with data from temp
    dirEntry *entryBuffer = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
    LBAread(entryBuffer, MBR_st->dirNumBlocks, temp->directoryStartLocation);
    int freeIndex = -1;

    //cycle through all entries in directory, till we find an empty one
    for (int i = 0; i < STARTING_NUM_DIR; i++)
    {
        if (entryBuffer[i].isBeingUsed == 0)
        {
            printf("FREEE INDEX: %d\n", i);
            freeIndex = i;
            break;
        }
    }

    //we did not find a free entry. Directory is full
    if (freeIndex == -1)
    {
        return -1;
    }

    //fill entry with new information
    entryBuffer[freeIndex].isBeingUsed = 1;
    printf("newName: %s\n", newName);
    strcpy(entryBuffer[freeIndex].name, newName);
    printf("entryBuffer[freeIndex].name = %s\n", entryBuffer[freeIndex].name);
    //we don't need to append a null terminator to a char* string
    entryBuffer[freeIndex].type = atoi("d");

    //create and initialize a directory for this new entry
    entryBuffer[freeIndex].childLBA = initDirectory(temp->directoryStartLocation);
    LBAwrite(entryBuffer, MBR_st->dirNumBlocks, temp->directoryStartLocation);

    //test this to make sure it created properly
    printf("\n\nPrinting parent directory of %s at: %ld\n", entryBuffer[freeIndex].name, temp->directoryStartLocation);
    for (int i = 0; i < 4; i++)
    {
        testOutput(&entryBuffer[i]);
    }
    printf("\n");

    free(entryBuffer); //we don't need you anymore, buffer

    return 0; //0 = success
}

int fs_remove_helper(dirEntry *deToRemove)
{
    dirEntry *entryBuff = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
    int blocks = MBR_st->dirNumBlocks;
    deToRemove->isBeingUsed = 0;
    strcpy(deToRemove->name, "deleted\n");
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
    char* newPath;
    short isAbsolute = 0;
    short isRelative = 0;

    //create directory entry iterator
    dirEntry* ptr = (dirEntry*)malloc(MBR_st->dirBufMallocSize);

    //parse path into tokens
    int numTokens = 0;
    char** tokens; 
    unsigned long startLBA; //depending on relative or absolute path, this will differ
    
    //case1: user enters /
    //if path is one char and that char is / go to root
    if((strlen(buf) == 1) && (strcmp(buf, "/") == 0))
    {
        /*
        printf("strlen(%s): %ld\n", buf, strlen(buf));
        printf("buf[0]: %s\n", &buf[0]);
        printf("user entered / so going back to root.\n");
        */

        //fill the buffer from root
        fdDirCWD->directoryStartLocation = MBR_st->rootDirectoryPos;
        fdDirCWD->streamCount = 0;
        fdDirCWD->dirEntryPosition = 0;
        strncpy(fdDirCWD->cwd_path, buf, strlen(buf)); //rename path in cwd
        printf("\nCWD: %s\nLBA: %ld\n", fdDirCWD->cwd_path, fdDirCWD->directoryStartLocation); 

        //deallocate 
        free(tokens);
        tokens = NULL;
        free(ptr);
        ptr = NULL;

        return 0;
    }

    //case2: user enters .
    else if((strlen(buf) == 1) && (buf[0] == '.'))
    {
        /*
        printf("strlen(%s): %ld\n", buf, strlen(buf));
        printf("buf[0]: %s\n", &buf[0]);
        printf("user entered . so we won't change cwd\n");
        */
        printf("\nCWD: %s\nLBA: %ld\n", fdDirCWD->cwd_path, fdDirCWD->directoryStartLocation); 

        //deallocate
        free(tokens);
        tokens = NULL;
        free(ptr);
        ptr = NULL;

        return 0;
    }

    //user enters a buf longer than one character
    else if(strlen(buf) > 1) 
    {
        /*printf("[442]strlen(%s): %ld\n", buf, strlen(buf));
        printf("[443]buf: %s\n", buf);
        */
        //case3: user enters ..- go up a directory
        if((strlen(buf) == 2) && (strcmp(buf, "..") == 0))
        {
            //printf("I'm going to go back a directory\n");
            LBAread(ptr, MBR_st->dirNumBlocks, fdDirCWD->directoryStartLocation);
            //printf("ptr[1].locationLBA: %ld\n", ptr[1].locationLBA);
            fdDirCWD->directoryStartLocation = ptr[1].locationLBA; //take the ptr stored in QQ entry
            printf("directoryStartLocation changed to %ld\n", fdDirCWD->directoryStartLocation);
            fdDirCWD->dirEntryPosition = 0;

            //deconstruct the cwd path
            tokens = tokenizePath(fdDirCWD->cwd_path, &numTokens);

            //reconstruct the path without the last dir name
            //printf("numTokens: %d\n", numTokens);
            numTokens--; //reduce numTokens by 1
            //printf("numTokens - 1 = %d\n", numTokens);
            newPath = "/";
            //printf("newPath: %s\n", newPath);
            //strcpy(newPath, "/"); //prepend the root dir path
            //printf("newPath: %s\n", newPath);
            if(numTokens > 0)
            {
                for(int i = 0; i < numTokens; i++)
                {
                    strcat(newPath, tokens[i]);
                    strcat(newPath, "/"); //add a trailing slash
                }
            }
            int newPathLength = strlen(newPath);
            //printf("newPathLength: %d\n", newPathLength);
            char lastChar = newPath[newPathLength - 1];
            //printf("lastChar: %c\n", lastChar);
            if(lastChar != '/') strcat(newPath, "/"); //add a trailing slash
            strcat(newPath, "\0");
            strcpy(fdDirCWD->cwd_path, newPath); //copy the new path over to the fdDir struct
            strcat(fdDirCWD->cwd_path, "\0");

            printf("\nCWD: %s\nLBA: %ld\n", fdDirCWD->cwd_path, fdDirCWD->directoryStartLocation); 

            //deallocate    
            newPath = NULL;
            free(tokens);
            tokens = NULL;
            free(ptr);
            ptr = NULL;

            return 0;
        }

        //we have a named path to deal with, parse buffer out into tokens
        tokens = tokenizePath(buf, &numTokens);
        printf("numTokens: %d\n", numTokens);
        printf("tokens[0]: %s\n", tokens[0]);

        //case4: absolute path- starts with /
        printf("buf[0]: %c\n", buf[0]);
        if(buf[0] == '/')
        {
            
            printf("[462]strlen(%s): %ld\n", buf, strlen(buf));
            printf("[462]buf[0]: %c\n", buf[0]);
            
            printf("[463]absolute path entered\n");
            isAbsolute = 1;
            startLBA = MBR_st->rootDirectoryPos; //we will start iterating from root
        }

        //case5: relative path- go down a directory
        printf("buf[0]: %c\n", buf[0]);
        if(buf[0] != '/')
        {
            printf("relative path entered\n");
            isRelative = 1;
            startLBA = fdDirCWD->directoryStartLocation;
            printf("512\n");
        }

        //start building our new path
        if(isRelative)
        {
            printf("[531]cwd_path: %s\n", fdDirCWD->cwd_path);
            //printf("cwd_path length: %ld\n", strlen(fdDirCWD->cwd_path));
            //printf("[528]newPath: %s\n", newPath);
            //char* temp;
            //strcpy(temp, fdDirCWD->cwd_path);
            //printf("temp: %s\n", temp);
            //newPath = temp;
            newPath = fdDirCWD->cwd_path;
            printf("[534] strncpy worked\n");
            printf("[535]newPath: ");
            printf("[536]%s\n", newPath); 
            if(strcmp(newPath, "/") == 0){
                printf("cwd is root\n");
            }
        }
        else if(isAbsolute)
        {
            printf("About to copy absolute path\n");
            //strcpy(newPath, buf); //our path should be exactly what was fed to us
            newPath = buf;
            printf("strcpy worked\nnewPath: %s\n", newPath);
            int size = strlen(newPath);
            char lastChar = newPath[size];
            printf("size: %d\n", size);
            printf("newPath[size]: %c\n", lastChar);
            //printf("newPath[size]: %c\n", newPath)
            //if(lastChar != '/') strcat(newPath, "/"); //add a trailing slash
        }        
        //start a while loop to go down directories until we're at the last
        for(int i = 0; i < numTokens; i++)
        {
            //read the cwd into dirEntry ptr
            LBAread(ptr, MBR_st->dirNumBlocks, startLBA);
            printf("[555]LBAread successful\n");

            //loop through all entries
            for(int j = 0; j < MBR_st->dirNumBlocks; j++)
            {
                //printf("[560] starting loop through entries\n");
                //printf("[561]tokens[%d]: %s\n", i, tokens[i]);
                //printf("[562]ptr[%d].name: %s\n", j, ptr[j].name);
                if(strcmp(tokens[i],ptr[j].name) == 0)
                {
                    //we found a match
                    printf("[564]We found a match\n");
                    if(isRelative)
                    {
                        //printf("[567]tokens[%d]: %s\n", i, tokens[i]);
                        if(newPath) strcat(newPath, tokens[i]);
                        else strcpy(newPath, tokens[i]);
                        strcat(newPath, "/");
                    }
                    //if we are at the end of our tokens, we have finished
                    if(i == (numTokens - 1))
                    { 
                        printf("We are at the last token\n");
                        //change the cwd data
                        fdDirCWD->directoryStartLocation = ptr[j].childLBA;
                        fdDirCWD->dirEntryPosition = ptr[j].entryIndex;
                        strcpy(fdDirCWD->cwd_path, newPath);
                        strcat(fdDirCWD->cwd_path, "\0");
                        /*printf("fdDirCWD->directoryStartLocation: %ld\n", fdDirCWD->directoryStartLocation);
                        printf("fdDirCWD->dirEntryPosition: %d\n", fdDirCWD->dirEntryPosition);
                        printf("fdDirCWD->cwd_path: %s\n", fdDirCWD->cwd_path);*/
                        printf("\nCWD: %s\nLBA: %ld\n", fdDirCWD->cwd_path, fdDirCWD->directoryStartLocation); 

                        //deallocate
                        free(tokens);
                        tokens = NULL;
                        free(ptr);
                        ptr = NULL;

                        return 0;
                    }
                }
                printf("newPath: %s\n", newPath);
            }
        }
        //we didn't find anything
        printf("%s not found.\n", buf);
    }//else if buf length > 1
    printf("invalid input for setCWD.\n\n");
    return -1;
}

int fs_isDir(char *path)
{
    //create a new path name and set it to point
    //to the last occurence of a slash
    char * newName = malloc(256);
    int slash = '/';
    newName = strrchr(path, slash); 

    //if we didn't find a / then simply copy the
    //path to newName and increment its start to 
    //the second character
    if (newName == NULL){
        newName = malloc(256);
        strcpy(newName, path);
    } else {
        newName++;
    }

    //if the path is literally just a slash, that
    //means we are at the root, so we are in a dir
    if (strcmp(path, "/") == 0){
        printf("\nis Root\n");
        return 1;
    }
    dirEntry *entryBuffer = (dirEntry *)malloc(MBR_st -> dirBufMallocSize);
    int blocks = MBR_st -> dirNumBlocks;
    fdDir *temp = tempDirectory(path, 1);
    LBAread(entryBuffer, blocks, temp -> directoryStartLocation);
    for (int i = 0; i < STARTING_NUM_DIR; i++) {
        if (strcmp(entryBuffer[i].name, newName) == 0)
        { 
           if(entryBuffer[i].type == 100) { //100 = ascii for 'd'
               return 1;
           } else {
               return 0;
           }
        }
    }
}

int fs_isFile(char *path)
{
    return !(fs_isDir(path));
}
