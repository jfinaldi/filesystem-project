#include "mfs.h"


fdDir *tempDirectory(char *path) {
    printf("\n\n----------------\nin tempDir\n"); 

    char temp[256]; //create a name for our temp directory path
    strncpy(temp, fdDirCWD -> cwd_path, sizeof(fdDirCWD -> cwd_path)); //copy over the path from cwd to temp name

    //we have an absolute path given
    if (strcmp(&path[0], "/") == 0) {
        strncpy(temp, path, sizeof(path)); 
    } 
    //we have a relative path given
    else {
        strcat(temp, path);  
    }
    
    int curr = MBR_st -> rootDirectoryPos; //start from the very first directory LBA block
    fdDir *ans = (fdDir*) malloc(sizeof(fdDir)); //this will be what we return

    //out path consists only of /, indicating we are at root dir
    if (strcmp(path, "/") == 0){
        printf("\nis Root\n"); 
        ans -> directoryStartLocation = MBR_st -> rootDirectoryPos; //assign our fdDir start loc to root start loc
        return ans; 
    }

    char * token = strtok(temp, "/"); //grab the very first directory name in our path
    int counter = 0; 
    int blocks = MBR_st -> dirNumBlocks; //total LBA blocks in our directory

    // loop through the string to extract all other tokens
    while( token != NULL ) {
            printf( "TOKEN! %s\n", token ); //printing each token
            printf("in loop: \n"); 
            dirEntry *entryBuffer = (dirEntry *)malloc(MBR_st -> dirBufMallocSize); 
            LBAread(entryBuffer, blocks, curr);  
            int flag = -1; 
            for (int i = 0; i < STARTING_NUM_DIR; i++) {
                if (strcmp(entryBuffer[i].name, token) == 0)
                {
                    printf("HI HELLO %d  < i %d\n", i, entryBuffer[i].childLBA); 
                    curr = entryBuffer[i].childLBA; 
                    flag = 0; 
                    break; 
                }
            }
            if (flag == -1) {
                printf ("not found\n"); 
                //return -1; 
            }
        
        token = strtok(NULL, "/");
        if (token == NULL) {
            ans -> directoryStartLocation = curr; 
        }
    }
    printf("-------------");
    return ans; 
}
fdDir *fs_opendir(const char *name)
{
    //we should know the global variable for fdDir object
    fdDir* result = (fdDir*)malloc(sizeof(fdDir));

    //malloc the dirItemInfo struct inside dirp
    result->dirItemInfo = (fd_diriteminfo*)malloc(sizeof(fd_diriteminfo));
    if (result->dirItemInfo == NULL)
	{
		printf("Error with malloc ln122.\n");
		return NULL;
	}
	else
		printf("Malloc succeeded\n\n");

    //create a dirEntry pointer, malloc dirBUfMallocSize bytes 
    dirEntry* ptr = (dirEntry*)malloc(MBR_st->dirBufMallocSize);
    if (ptr == NULL)
	{
		printf("Error with malloc ln60.\n");
		return -1;
	}
	else
		printf("Malloc succeeded\n\n");
    
    printf("\nname: %s", name); 
    fdDir* temp = (fdDir*)malloc(sizeof(fdDir));
    temp = tempDirectory(name); 
    temp -> streamCount = 0; 
    printf("\ntemp->directoryStartLocation: %d", temp->directoryStartLocation);
    return temp;

    //do LBA read to populate the buffer from our fdDirCWD->directoryStartLocation
    LBAread(ptr, MBR_st->dirNumBlocks, temp->directoryStartLocation);

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
    printf("\n\nin read------"); 

    //malloc a dirEntry pointer buffer
    dirEntry* ptr = (dirEntry*)malloc(MBR_st->dirBufMallocSize);
    if (ptr == NULL)
	{
		printf("Error with malloc.\n");
		return NULL;
	}
	else
		printf("\nMalloc succeeded\n");

    //fill our buffer with a directory
    LBAread(ptr, MBR_st->dirNumBlocks, dirp->directoryStartLocation);
    
    //populate dirItemInfo with information from dirp
    while (ptr[dirp->streamCount].isBeingUsed == 0 && dirp -> streamCount < 50) {
        dirp -> streamCount++;
    }
    if (ptr[dirp->streamCount].isBeingUsed == 1) {
        dirp->dirItemInfo->fileType = ptr[dirp->streamCount].type;
        strcpy(dirp->dirItemInfo->d_name, ptr[dirp->streamCount].name);
        dirp -> streamCount++;
        return result; 
    } else {
        return NULL; 
    }
    
     
    
    //return
    //free(ptr);
    return NULL;
}

//this function will free all memory for dirp
int fs_closedir(fdDir *dirp)
{
    //free dirItemInfo
    if(dirp->dirItemInfo) free(dirp->dirItemInfo);
    if(dirp->dirItemInfo)
    {
        printf("\nERROR, dirItemInfo not deallocated.\n");
        return 1;
    }
    
    //free all memory
    free(dirp);
    if(dirp)
    {
        printf("\nError fsORC.c, failed attempt to free dirp, ln 155\n");
        return 1;
    }
    return 0;
}



int fs_mkdir(const char *pathname, mode_t mode){
    char * pathWithoutName = malloc (sizeof(pathname)); 
    char * newName = malloc(256); 
    int slash = '/';  
    newName = strrchr(pathname, slash);
    if (newName == NULL){
        newName = malloc(256);
        strcpy(newName, pathname);
    } else {
        newName++;
    }
     
    printf("new name %s", newName); 
    fdDir *temp = tempDirectory(pathname); 
    printf("\n\nHELLLO, dir start loc: %d", temp -> directoryStartLocation);
    dirEntry *entryBuffer = (dirEntry *)malloc(MBR_st -> dirBufMallocSize); 
    int blocks = MBR_st -> dirNumBlocks;
    LBAread(entryBuffer, blocks, temp -> directoryStartLocation); 
    int freeIndex = -1; 
    for (int i = 0; i < STARTING_NUM_DIR; i++) {
        if ( entryBuffer[i].isBeingUsed == 0)
        {
            printf("\nFREEE INDEX: %d", i); 
           freeIndex = i;  
           break;
        }
    } 
    if (freeIndex == -1) {
        return -1;
    }
    entryBuffer[freeIndex] . isBeingUsed = 1; 
    //printf("\nTRY TO STRCOPY: %s", pathname);
    //strcpy(entryBuffer[freeIndex].name, newName);
    //entryBuffer[freeIndex].name[sizeof(newName)] = "\0";
    
    entryBuffer[freeIndex].childLBA =  initDirectory(temp -> directoryStartLocation); 
    LBAwrite(entryBuffer, blocks, temp -> directoryStartLocation);
    printf("\nSTARTINGBLOCK: %d", temp -> directoryStartLocation); 
    for (int i = 0; i < 6; i++) {
        testOutput(&entryBuffer[i]);
    }
    //free(entryBuffer); 
    
    return 0; 

}
int fs_remove_helper(dirEntry *deToRemove) {
    dirEntry *entryBuff = (dirEntry *)malloc(MBR_st -> dirBufMallocSize); 
    int blocks = MBR_st -> dirNumBlocks;
    deToRemove -> isBeingUsed = 0; 
    strcpy(deToRemove -> name, "\ndeleted");
    printf("\nCHILDLABA, %d", deToRemove -> childLBA); 
    LBAread(entryBuff, blocks, deToRemove -> childLBA); 
    for (int i = 2; i < STARTING_NUM_DIR; i++) {
        // if (0) {
        //     entryBuffer[i] . isBeingUsed = 0;
        //     // deletefile();
        //     // free_mem(fileStartLBA, fileLength); 
        // } else 
        if (entryBuff[i].isBeingUsed == 1) {
            printf("\nwhy is this printing %d??", i); 
            fs_remove_helper(&entryBuff[i]);
        }
    }
    LBAwrite(entryBuff, blocks, deToRemove -> childLBA);
    free_mem(deToRemove -> childLBA, blocks);
     
    //free(entryBuffer);  
}
int fs_rmdir(const char *pathname){
    dirEntry *entryBuffer = (dirEntry *)malloc(MBR_st -> dirBufMallocSize); 
    int blocks = MBR_st -> dirNumBlocks;
    LBAread(entryBuffer, blocks, fdDirCWD -> directoryStartLocation); 
    int remove_index = -1;
    printf("\nTRYING TO DELETE: %s", pathname); 
    for (int i = 0; i < STARTING_NUM_DIR; i++) {
        if (strcmp(entryBuffer[i].name, pathname) == 0)
        {
           remove_index = i;
           printf("\ndelete index: %d", i); 
           break;  
        }
    }
    fs_remove_helper(&entryBuffer[remove_index]);
    LBAwrite(entryBuffer, blocks, fdDirCWD -> directoryStartLocation); 
    //free(entryBuffer); 
    for (int i = 0; i < 6; i++) {
        testOutput(&entryBuffer[i]);
    }

}
char *fs_getcwd(char *buf, size_t size){
    
    char *path = malloc(256);
    strncpy(path, fdDirCWD -> cwd_path, sizeof(fdDirCWD -> cwd_path));
    printf("\nIN GETCWD: %s", path); 
    return path;  
}
int fs_setcwd(char *buf){
     
    char temp[256]; 
    strncpy(temp, fdDirCWD -> cwd_path, sizeof(fdDirCWD -> cwd_path)); 
     printf("\nSETTT: %s", fdDirCWD -> cwd_path);
    if (strcmp(&buf[0], "/") == 0) {
        strncpy(temp, buf, sizeof(buf)); 
    } else {
        strcat(temp, buf);  
        printf("\nSETTT: %s", temp);
    }
    
    int curr = MBR_st -> rootDirectoryPos; 
    char * token = strtok(temp, "/");
    int counter = 0; 
    char ans[20][256];   
    int blocks = MBR_st -> dirNumBlocks;
   // loop through the string to extract all other tokens
   while( token != NULL ) {
        printf( "\n\nTOKEN! %s\n", token ); //printing each token
        printf("\nin loop---"); 
        dirEntry *entryBuffer = (dirEntry *)malloc(MBR_st -> dirBufMallocSize); 
        LBAread(entryBuffer, blocks, curr);  
        int flag = -1; 
        for (int i = 0; i < STARTING_NUM_DIR; i++) {
            if (strcmp(entryBuffer[i].name, token) == 0)
            {
                printf("\nHI HELLO %d  < i %d", i, entryBuffer[i].childLBA); 
                curr = entryBuffer[i].childLBA; 
                flag = 0; 
                break; 
            }
        }
        if (flag == -1) {
            printf ("\nnot found"); 
            return -1; 
        }
    
      token = strtok(NULL, "/");
   }
    
    fdDirCWD -> directoryStartLocation = curr; 
    if (strcmp(&buf[0], "/") == 0) {
        strncpy(fdDirCWD -> cwd_path, buf, sizeof(buf)); 
        strncpy(fdDirCWD -> cwd_path, "\0", 1); 
    } else {
        
        strcat(fdDirCWD -> cwd_path, buf); 
        strcat(fdDirCWD -> cwd_path, "/\0"); 
    }
    printf("\nCURRCWD  %s beep", fdDirCWD -> cwd_path); 
    return 0; 
}
char parsePath(char *buf) {
    char * token = strtok(buf, "/");
    int counter = 0; 
    char ans[20][256];   
   // loop through the string to extract all other tokens
   while( token != NULL ) {
      printf( "\nTOKEN! %s\n", token ); //printing each token
      strcpy(ans[counter], token); 
      token = strtok(NULL, "/");
   }
   return ans; 
}

