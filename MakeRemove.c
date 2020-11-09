#include "mfs.h"

int fs_mkdir(const char *pathname, mode_t mode){
    printf("HELLLO, dir start loc, %d", fdDirCWD -> directoryStartLocation);
    dirEntry *entryBuffer = (dirEntry *)malloc(MBR_st -> dirBufMallocSize); 
    int blocks = MBR_st -> dirNumBlocks;
    LBAread(entryBuffer, blocks, fdDirCWD -> directoryStartLocation); 
    int freeIndex = -1; 
    for (int i = 0; i < STARTING_NUM_DIR; i++) {
        if ( entryBuffer[i].isBeingUsed == 0)
        {
            printf("FREEE INDEX %d", i); 
           freeIndex = i;  
           break;
        }
    } 
    if (freeIndex == -1) {
        return -1;
    }
     entryBuffer[freeIndex] . isBeingUsed = 1; 
    printf("TRY TO STRCOPY %s", pathname);
    strcpy(entryBuffer[freeIndex].name, pathname);
    entryBuffer[freeIndex].name[sizeof(pathname)] = "\0";
    
    entryBuffer[freeIndex].childLBA =  initDirectory(fdDirCWD -> directoryStartLocation); 
    LBAwrite(entryBuffer, blocks, fdDirCWD -> directoryStartLocation);
    printf("STARTINGBLOCK %d", fdDirCWD -> directoryStartLocation); 
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
    strcpy(deToRemove -> name, "deleted");
    printf("CHILDLABA, %d", deToRemove -> childLBA); 
    LBAread(entryBuff, blocks, deToRemove -> childLBA); 
    for (int i = 2; i < STARTING_NUM_DIR; i++) {
        // if (0) {
        //     entryBuffer[i] . isBeingUsed = 0;
        //     // deletefile();
        //     // free_mem(fileStartLBA, fileLength); 
        // } else 
        if (entryBuff[i].isBeingUsed == 1) {
            printf("why is this printing %d", i); 
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
    printf("TRYING TO DELETE %s", pathname); 
    for (int i = 0; i < STARTING_NUM_DIR; i++) {
        if (strcmp(entryBuffer[i].name, pathname) == 0)
        {
           remove_index = i;
           printf("delete index, %d", i); 
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
    printf("IN GETCWD"); 
    char path[256];
    strncpy(path, fdDirCWD -> cwd_path, sizeof(fdDirCWD -> cwd_path));
    return path;  
}
int fs_setcwd(char *buf){
     
    char temp[256]; 
    strncpy(temp, fdDirCWD -> cwd_path, sizeof(fdDirCWD -> cwd_path)); 
     printf("SETTT %s", fdDirCWD -> cwd_path);
    if (strcmp(&buf[0], "/") == 0) {
        strncpy(temp, buf, sizeof(buf)); 
    } else {
        strcat(temp, buf);  
        printf("SETTT %s", temp);
    }
    
    int curr = MBR_st -> rootDirectoryPos; 
    char * token = strtok(temp, "/");
    int counter = 0; 
    char ans[20][256];   
    int blocks = MBR_st -> dirNumBlocks;
   // loop through the string to extract all other tokens
   while( token != NULL ) {
        printf( "TOKEN! %s\n", token ); //printing each token
        printf("in loop"); 
        dirEntry *entryBuffer = (dirEntry *)malloc(MBR_st -> dirBufMallocSize); 
        LBAread(entryBuffer, blocks, curr);  
        int flag = -1; 
        for (int i = 0; i < STARTING_NUM_DIR; i++) {
            if (strcmp(entryBuffer[i].name, token) == 0)
            {
                printf("HI HELLO %d  < i %d", i, entryBuffer[i].childLBA); 
                curr = entryBuffer[i].childLBA; 
                flag = 0; 
                break; 
            }
        }
        if (flag == -1) {
            printf ("not found"); 
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
    printf("CURRCWD  %s beep", fdDirCWD -> cwd_path); 
    return 0; 
}
char parsePath(char *buf) {
    char * token = strtok(buf, "/");
    int counter = 0; 
    char ans[20][256];   
   // loop through the string to extract all other tokens
   while( token != NULL ) {
      printf( "TOKEN! %s\n", token ); //printing each token
      strcpy(ans[counter], token); 
      token = strtok(NULL, "/");
   }
   return ans; 
}
//fdDir tempDirectory()


