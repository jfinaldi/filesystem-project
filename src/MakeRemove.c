/**************************************************************
* Class:  CSC-415-02 
* Name: Lauren/Jenn (DreamTeam)
* Student ID:
* Project: File System Project
*
* File: MakeRemove.c
*
* Description: Interface for Shell Functions
*           Implements functions for the directory services
*           including making, removing, opening, reading,
*           and necessary basic functions that directory
*           shell commands require
*
**************************************************************/

#include "mfs.h"

char** tokenizePath(char* path, int* i)
{
    char** result = (char**)malloc(strlen(path) * sizeof(char**));
    
    // loop through the string to extract all other tokens
    char* token = strtok(path, "/");
    (*i) = 0;
    while (token != NULL)  {
        result[*i] = token;
        (*i)++;
        token = strtok(NULL, "/");
    }  
    return result;
}

//removes . and .. and directories corresponding to .. from path
//returns shortest absolute path to CWD
//46 is ASCII for .
char * pathResolver(char *path) { 
    int numTokens = 0;
    char** tokens = tokenizePath(path, &numTokens);
    char * returnPath = malloc(256); 
    strcpy(returnPath, "/");

    //loop through tokens
    for (int i = numTokens - 1; i >= 0 ; i--) {
        int count = 0; 
        _Bool lastLetterDot = tokens[i - count][(strlen(tokens[i - count])-1)] == 46; 
        _Bool secondTolastLetterDot = tokens[i - count][(strlen(tokens[i - count])-2)] == 46;

        //remove ..s
        while (i - count!= -1 &&  lastLetterDot && secondTolastLetterDot) {
            strcpy(tokens[i - count], "\0");
            count++;
            if (i - count == -1) {
                break; 
            }
            lastLetterDot = tokens[i - count][(strlen(tokens[i - count])-1)] == 46;
            secondTolastLetterDot = tokens[i - count][(strlen(tokens[i - count])-2)] == 46;
        } 

        //remove dirs corresponding to ..s
        for (int k = 0; k  < count; k++) {
            int toRemove = i - count - k; 
            if (toRemove >= 0) {
                strcpy(tokens[toRemove], "\0");
            }
        }

        //remove .s
        if (lastLetterDot && !secondTolastLetterDot){
            strcpy(tokens[i], "\0");
        }
    }
    //re-concat path
    for (int i = 0; i < numTokens ; i++) {
        strcat(returnPath, tokens[i]);
        if (strcmp(tokens[i], "\0") != 0 && i != numTokens - 1) {
            
            strcat(returnPath, "/\0");
        }
    } 
    return returnPath; 
}

fdDir *tempDirectory(const char *path, int needLast, char *name) {

    char temp[256];
    _Bool isAbsolute = strncmp(&path[0], "/", 1) == 0;

    //temp is path if absolute, otherwise concat with CWD
    if (isAbsolute) {
        strncpy(temp, path, sizeof(char*) * sizeof(path));
    } else {
        strncpy(temp, fdDirCWD->cwd_path, sizeof(fdDirCWD->cwd_path));
        strcat(temp, path);
    }

    int curr = MBR_st->rootDirectoryPos;
    int last = curr;
    fdDir *resultDir = (fdDir *)malloc(sizeof(fdDir));
    _Bool isRoot =  strcmp(path, "/") == 0;
    strcpy(resultDir -> cwd_path, temp); 

    //early return for Root
    if (isRoot) {
        resultDir->directoryStartLocation = MBR_st->rootDirectoryPos;
        return resultDir;
    }

    int numTokens = 0;
    char** tokens = tokenizePath(temp, &numTokens);
    int blocks = MBR_st->dirNumBlocks;
    int notFoundCount = 0; 

    //loop through tokens
    int isFile = 0; 
    for (int k = 0; k < numTokens; k++) {
        dirEntry *entryBuffer = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
        LBAread(entryBuffer, blocks, curr);
        
        int found = -1;
        //check children for currToken, note if not found
        for (int i = 0; i < STARTING_NUM_DIR; i++) {
            if (name != NULL) {
                if (strcmp(entryBuffer[i].name, name) == 0) {   
                    printf ("name already exists, please try again\n"); 
                    resultDir-> directoryStartLocation = DEFAULT_LBA;
                    return resultDir;
                }
            }
            
            if (strcmp(entryBuffer[i].name, tokens[k]) == 0) {
                last = curr;
                curr = entryBuffer[i].childLBA;
                if (entryBuffer[i].childLBA == DEFAULT_LBA) {
                    curr = last;
                }
                found = 0;
            }
            
        }
        if (found == -1) {
            notFoundCount ++; 
        }
        //if more than 1 tokens aren't found return not found 
        if (notFoundCount > 1) {
            printf ("not found in temp\n"); 
            resultDir-> directoryStartLocation = DEFAULT_LBA;
            return resultDir; 
        }
        if(entryBuffer) {
            free(entryBuffer);
            entryBuffer = NULL;
        }
        if(entryBuffer) 
        {
            printf("Error deallocating entryBuffer in tempDirectory makeremove.c\n");
        }

    }
    // || (needLast == 0 && notFoundCount == 0)
    if (needLast == 1){
        resultDir->directoryStartLocation = last;
    } else {
        resultDir->directoryStartLocation = curr;
    }
    
    return resultDir;
}

fdDir *fs_opendir(const char *name)
{
    //printf("name to open %s\n", name); 
    fdDir *result = (fdDir *)malloc(sizeof(fdDir));
    dirEntry *ptr = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
    if (ptr == NULL){
        return ((fdDir *)-1);
    }
    fdDir *temp = (fdDir *)malloc(sizeof(fdDir));
    temp = tempDirectory(name, 0, NULL);
    if (temp -> directoryStartLocation == DEFAULT_LBA) {
            printf("Error: not a valid path or name\n"); 
            return NULL;
    }
    temp->streamCount = 0;
    return temp;
}

struct fs_diriteminfo *fs_readdir(fdDir *dirp) {
    
    //malloc new item info 
    struct fs_diriteminfo *result = (struct fs_diriteminfo *)malloc(sizeof(struct fs_diriteminfo));
    if (result == NULL) {
        printf("malloc err\n");
        return ((struct fs_diriteminfo *)-1);
    }
   
   //malloc and put dir in buffer
    dirEntry *ptr = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
    if (ptr == NULL) {
        printf("malloc err\n");
        return ((struct fs_diriteminfo *)-1);
    }

    LBAread(ptr, MBR_st->dirNumBlocks, dirp->directoryStartLocation);
 
    
    //ignore empty Dirs, incrememnt StreamCount
    while (ptr[dirp->streamCount].isBeingUsed == 0 && dirp->streamCount < STARTING_NUM_DIR) {
        dirp->streamCount++;
    }
    //set info to return if being used, increment StreamCount
    if (!ptr[dirp->streamCount].isBeingUsed == 0) {
        result->fileType = ptr[dirp->streamCount].type;
        if(ptr[dirp->streamCount].type == 'D') result->d_size = MBR_st->dirBufMallocSize;
        else result->d_size = ptr[dirp->streamCount].sizeOfFile;
        result->d_createtime = ptr[dirp->streamCount].dateCreated;
        strcpy(result->d_name, ptr[dirp->streamCount].name);
        dirp->streamCount++;
        return result;
    }
    else {
        return NULL;
    }
}

//this function will free all memory for dirp
int fs_closedir(fdDir *dirp)
{
    //if dirp is already null, don't try to free anything
    if (dirp == NULL) {
        printf("dirp is NULL.\n");
        return 0; 
    }

    //free all memory
    if (dirp) {
        free(dirp);
        dirp = NULL;
        if (dirp)
        {
            printf("Error MakeRemove.c, failed attempt to free dirp, ln 249\n");
            return 1;
        }
    }
         
    return 0;
}

int fs_mkdir(const char *pathname, mode_t mode)
{
    //isolate name of new Dir
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

    //resolve pathname and load buffer with Dir
    fdDir *temp = tempDirectory(pathname, 0, newName);
    if (temp -> directoryStartLocation == DEFAULT_LBA) {
            printf("Error: not a valid path or name\n"); 
            return -1;
    }
    dirEntry *entryBuffer = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
    int blocks = MBR_st->dirNumBlocks;
    LBAread(entryBuffer, blocks, temp->directoryStartLocation);

    int freeIndex = -1;
    //find free index
    for (int i = 0; i < STARTING_NUM_DIR; i++) {
        
        if (strcmp(entryBuffer[i].name, newName) == 0) {
            printf("File or directory with that name already exists, please use a different name\n"); 
            return -1;
        }
        if (entryBuffer[i].isBeingUsed == 0) {
            freeIndex = i;
        }
    }
    if (freeIndex == -1){
        printf("Directory is full ( ͡° ͜ʖ ͡°)\n");
        return -1;
    }

    //Set info to return 
    entryBuffer[freeIndex].isBeingUsed = 1;
    strcpy(entryBuffer[freeIndex].name, newName);
    entryBuffer[freeIndex].type = 'D';
    entryBuffer[freeIndex].childLBA = initDirectory(temp->directoryStartLocation);
    LBAwrite(entryBuffer, blocks, temp->directoryStartLocation);

    free(entryBuffer); //we don't need you anymore, buffer

    return 0; //0 = success
}

int fs_remove_helper(dirEntry *deToRemove)
{
    //set toRemove to be empty 
    dirEntry *entryBuff = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
    int blocks = MBR_st->dirNumBlocks;
    deToRemove->isBeingUsed = 0;
    strcpy(deToRemove->name, "-----");

    //68 ASCII for "D", remove metaData if File
    if (deToRemove -> type != 68) {
        free_mem(deToRemove->locationMetadata, 512 * 20);
        wipeExtents(deToRemove); 
    } else {
        LBAread(entryBuff, blocks, deToRemove->childLBA);
        for (int i = 2; i < STARTING_NUM_DIR; i++) {
            if (entryBuff[i].isBeingUsed == 1) {
                fs_remove_helper(&entryBuff[i]);
            }
        }
        LBAwrite(entryBuff, blocks, deToRemove->childLBA);
        free_mem(deToRemove->childLBA, blocks);
    }
    
}

int fs_rmdir(const char *pathname)
{
    //resolve name of to remove
    char *nameToRemove = malloc(256);
    nameToRemove = strrchr(pathname, '/');
    if (nameToRemove == NULL) {
        nameToRemove = malloc(256);
        strcpy(nameToRemove, pathname);
    } else {
        nameToRemove++;
    }

    //set buffer and check if valid
    dirEntry *entryBuffer = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
    int blocks = MBR_st->dirNumBlocks;
    pathname = (char *) pathname;
    fdDir *temp = tempDirectory(pathname, 1, NULL);
    if (temp -> directoryStartLocation == DEFAULT_LBA) {
            printf("Error: not a valid path or name\n"); 
            return -1;
    }
    //printf("IN DELETE %s, %s", temp -> cwd_path, fdDirCWD -> cwd_path);
    if (strncmp(temp -> cwd_path, fdDirCWD -> cwd_path, strlen(temp -> cwd_path)) == 0) {
        printf("Error, you are removing the directory you are in!\n"); 
        return -1;
    }
    LBAread(entryBuffer, blocks, temp->directoryStartLocation);

    //find index to remove
    int remove_index = -1;
    for (int i = 0; i < STARTING_NUM_DIR; i++) {
        if (strcmp(entryBuffer[i].name, nameToRemove) == 0)
        {
            remove_index = i;
            break;
        }
    }

    //start recurisve removal and then write
    fs_remove_helper(&entryBuffer[remove_index]);
    LBAwrite(entryBuffer, blocks, temp->directoryStartLocation);

    //printf("BEFORE DEGRAG"); 
    //defrag(MBR_st -> rootDirectoryPos); 

    printf("Successful removal...\n");
    return 0;
}

char *fs_getcwd(char *buf, size_t size)
{
    char *path = malloc(256);
    strncpy(path, fdDirCWD->cwd_path, sizeof(fdDirCWD->cwd_path));
    strcat(path, "\0");  
    return path;
}

int fs_setcwd(char *buf)
{
   //printf ("insetcwd with %s\n", buf); 
    char temp[256];

    //if absolute temp is buf, otherwise concat with CWD
    int isAbsolute = strncmp(&buf[0], "/", 1) == 0; 
    strncpy(temp, fdDirCWD->cwd_path, sizeof(fdDirCWD->cwd_path));
    if (isAbsolute) {
        strncpy(temp, buf, sizeof(char*) * sizeof(buf));
    }
    else {
        strcat(temp, buf);
    }

    int curr = MBR_st->rootDirectoryPos;
    int numTokens = 0;
    char** tokens = tokenizePath(temp, &numTokens);
    int counter = 0;
    char resultDir[20][256];
    int blocks = MBR_st->dirNumBlocks;

    //loop looking for correct dir
    for (int k = 0; k < numTokens; k++) {
        //set buffer
        dirEntry *entryBuffer = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
        LBAread(entryBuffer, blocks, curr);
        _Bool found = 0;

        //find dir with same name as token and chnge curr to that dir's childLoc
        for (int i = 0; i < STARTING_NUM_DIR; i++) {
            if (strcmp(entryBuffer[i].name, tokens[k]) == 0) {
                curr = entryBuffer[i].childLBA;
                found = 1;
                break;
            }
        }
        if (found == 0) {
            printf("Error: not found in setcwd\n");
            return -1;
        }
    }

    //set info 
    fdDirCWD->directoryStartLocation = curr;
    
    //if is absolute copy, if not concat
    if (isAbsolute) {
        strncpy(fdDirCWD->cwd_path, buf, sizeof(buf) * sizeof(char*));
    } else {
        strcat(fdDirCWD->cwd_path, buf);
    }
    
    //resolve path, add ending slash if needed
    char * newPath = pathResolver(fdDirCWD->cwd_path);
    strncpy(fdDirCWD->cwd_path, newPath, sizeof(newPath) * sizeof(char*));
    _Bool isRoot = strcmp(fdDirCWD->cwd_path, "/") == 0; 
    _Bool lastIsSlash =  strcmp(&fdDirCWD->cwd_path[strlen(fdDirCWD->cwd_path) - 1], "/") == 0; 
    if (!isRoot && !lastIsSlash) {
        strcat(fdDirCWD->cwd_path, "/");
    }

    return 0;

}

int fs_isDir(char *path)
{
    char *newName = malloc(256);
    char* ptr;
    int slash = '/';
    ptr = strrchr(path, slash); //find the last slash

    //path name has no slashes at all
    if (ptr == NULL)
        strcpy(newName, path);

    //we found a last slash
    else
    {
        ptr++;
        strcpy(newName, ptr);
    }

    //if the path is literally just a slash, that
    //means we are at the root, so we are in a dir
    if (strcmp(path, "/") == 0){
        return 1;
    }
    dirEntry *entryBuffer = (dirEntry *)malloc(MBR_st -> dirBufMallocSize);
    int blocks = MBR_st -> dirNumBlocks;
    fdDir *temp = tempDirectory(path, 1, NULL);
    if (temp -> directoryStartLocation == DEFAULT_LBA) {
        printf("not a valid path or name\n"); 
        return -1;
    }
    LBAread(entryBuffer, blocks, temp -> directoryStartLocation);
    for (int i = 0; i < STARTING_NUM_DIR; i++) {
        if (strcmp(entryBuffer[i].name, newName) == 0)
        { 
           if(entryBuffer[i].type == 'D') {
               return 1;
           } else {
               return 0;
           }
        }
    }
    printf("nothing found in isDir"); 
    return 0;
}

int fs_isFile(char *path)
{
    return !fs_isDir(path);
}
int fs_delete(char *filename) {
    fs_rmdir(filename); 
}

int fs_stat(const char *path, struct fs_stat *buf)
{
    char *dirName = malloc(256);
    dirName = strrchr(path, '/');
    if (dirName == NULL) {
        dirName = malloc(256);
        strcpy(dirName, path);
    } else {
        dirName++;
    }

    //set buffer and check if valid
    dirEntry *entryBuffer = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
    int blocks = MBR_st->dirNumBlocks;
    path = (char *) path;
    fdDir *temp = tempDirectory(path, 1, NULL);
    if (temp -> directoryStartLocation == DEFAULT_LBA) {
        printf("Error: not a valid path or name\n"); 
        return -1;
    }
    LBAread(entryBuffer, blocks, temp->directoryStartLocation);
    
    if(entryBuffer->type != 'D') {
        buf->st_size = entryBuffer->sizeOfFile;
        
    }
    buf -> st_size = MBR_st -> dirBufMallocSize; //needs to be changed for file type
    buf -> st_blksize = MBR_st -> blockSize; 
	buf -> st_blocks = MBR_st -> dirNumBlocks;	  
	buf -> st_accesstime  = entryBuffer -> dateAccessed; 
	buf  -> st_modtime	= entryBuffer -> dateModified; 
    buf -> st_blksize = MBR_st -> blockSize;  
    buf -> type = entryBuffer -> type;
	buf -> st_createtime = entryBuffer -> dateCreated;/* time of last status change */
}

int fs_mvdir(char *srcPath, char *destPath) {
    char *destName = malloc(256);
    char* ptr;
    int slash = '/';
    ptr = strrchr(destPath, slash); //find the last slash

    //path name has no slashes at all
    if (ptr == NULL)
        strcpy(destName, destPath);

    //we found a last slash
    else
    {
        ptr++;
        strcpy(destName, ptr);
    }
    
    char *srcName = malloc(256);
    char* ptr2;
    ptr2 = strrchr(srcPath, slash); //find the last slash

    //path name has no slashes at all
    if (ptr2 == NULL)
        strcpy(srcName, srcPath);

    //we found a last slash
    else
    {
        ptr2++;
        strcpy(srcName, ptr2);
    }
    //printf("src name %s\n", srcName);

    
    dirEntry *entryBufferSrc = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
    dirEntry *entryBufferDest = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
    int blocks = MBR_st->dirNumBlocks;
    fdDir *tempSrc = tempDirectory(srcPath, 1, NULL);
    fdDir *tempDest = NULL;
    if (fs_isDir(destPath)) {
        //printf("IT IS DIR"); 
        tempDest = tempDirectory(destPath, 1, NULL);
    }  else {
        //printf("NOT DIR"); 
        tempDest = tempDirectory(destPath, 0, NULL);
    }
    if (strncmp(tempSrc -> cwd_path, fdDirCWD -> cwd_path, strlen(tempSrc -> cwd_path)) == 0) {
        //printf("CANT MOVE, YOU ARE IN THAT DIRECTORY!"); 
        return -1;
    }
    if (tempSrc -> directoryStartLocation == DEFAULT_LBA) {
            //printf("not a valid source path or name"); 
            return -1;
    }
    LBAread(entryBufferSrc, blocks, tempSrc->directoryStartLocation);

    int src_index = -1;
    for (int i = 0; i < STARTING_NUM_DIR; i++) {
        if (strcmp(entryBufferSrc[i].name, srcName) == 0)
        {
            src_index = i;
            break;
        }
    }
    if (src_index == -1 ) {
        //printf("no such file or directory exists as source");
    }
    entryBufferSrc[src_index].isBeingUsed = 0; 
    LBAwrite(entryBufferSrc, blocks, tempSrc -> directoryStartLocation);
    if (tempDest -> directoryStartLocation == DEFAULT_LBA) {
        printf("Error: not a valid destination path or name\n"); 
        return -1;
    }
    LBAread(entryBufferDest, blocks, tempDest->directoryStartLocation);
    
    
    int dest_index = -1;
    for (int i = 0; i < STARTING_NUM_DIR; i++) {
        if (strcmp(entryBufferDest[i].name, destName) == 0)
        {
            dest_index = i;
            break;
        }
    }
    int free_index = -1;
   
    if (dest_index == -1 ) {
        //printf("\nno such file or directory--must make new");
         
        //look for a free index
        for (int i = 0; i < STARTING_NUM_DIR; i++) {
            if (entryBufferDest[i].isBeingUsed == 0)
            {
                free_index = i;
            }
            if (strcmp(entryBufferDest[i].name, destName) == 0) {
                printf("File with that name already exists, please use a different name\n"); 
                return -1;
            }
        } 
        if (free_index == -1) {
            printf("Error: out of space\n"); 
            return -1;
        }

        //copy over relevant data
        entryBufferDest[free_index] = entryBufferSrc[src_index]; 
        strcpy(entryBufferDest[free_index].name, destName);
        entryBufferDest[free_index].isBeingUsed = 1; 
        LBAwrite(entryBufferDest, blocks, tempDest -> directoryStartLocation);

    } else if (entryBufferDest[dest_index].type == 'D') {
        //printf("putting in directory!!"); 
        dirEntry *entryBufferFinal = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
        LBAread(entryBufferFinal, blocks, entryBufferDest[dest_index].childLBA);
        for (int i = 0; i < STARTING_NUM_DIR; i++) {
            if (strcmp(entryBufferFinal[i].name, destName) == 0) {
                printf("File with that name already exists, please use a different name\n"); 
                return -1;
            }
            if (entryBufferFinal[i].isBeingUsed == 0)
            {
                free_index = i;
            }
        } 
        if (free_index == -1) {
            printf("Error: out of space\n"); 
            return -1;
        }
        entryBufferFinal[free_index] = entryBufferSrc[src_index];
        entryBufferFinal[free_index].isBeingUsed = 1;
        LBAwrite(entryBufferFinal, blocks, entryBufferDest[dest_index].childLBA);
    } else {
        entryBufferDest[dest_index] = entryBufferSrc[src_index];
        entryBufferDest[dest_index].isBeingUsed = 1;
        LBAwrite(entryBufferDest, blocks, tempDest -> directoryStartLocation);
    }
    
}