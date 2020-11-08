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
int fs_remove_helper(int blockToRemove) {
    dirEntry *entryBuffer = (dirEntry *)malloc(sizeof(dirEntry) * STARTING_NUM_DIR); 
    int blocks = (sizeof(dirEntry) * STARTING_NUM_DIR) / MBR_st -> blockSize + 1;
    LBAread(entryBuffer, blocks, blockToRemove); 
    for (int i = 0; i < STARTING_NUM_DIR; i++) {
        if (entryBuffer[i] . isFile == 1) {
            entryBuffer[i] . isBeingUsed = 0;
            // deletefile();
            // free_mem(fileStartLBA, fileLength); 
        } else if (entryBuffer[i]. isBeingUsed == 1) {
            entryBuffer[i] . isBeingUsed = 0;
            fs_remove_helper(entryBuffer[i] . locationLBA);
        }
    }
    free_mem(blockToRemove, blocks);
     
    free(entryBuffer);  
}
int fs_rmdir(const char *pathname){
    dirEntry *entryBuffer = (dirEntry *)malloc(sizeof(dirEntry) * STARTING_NUM_DIR); 
    int blocks = (sizeof(dirEntry) * STARTING_NUM_DIR) / MBR_st -> blockSize + 1;
    LBAread(entryBuffer, blocks, fdDirCWD -> directoryStartLocation); 
    int remove_index = -1;
    for (int i = 0; i < STARTING_NUM_DIR; i++) {
        if (strcmp(entryBuffer[i] . name, pathname) == 0)
        {
           remove_index = i;  
        }
    }
    fs_remove_helper(entryBuffer[remove_index] . locationLBA);
    free(entryBuffer); 

}


