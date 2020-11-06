#include "mfs.h"


int fs_mkdir(const char *pathname, mode_t mode){
    printf("HELLLO");
    dirEntry *entryBuffer = (dirEntry *)malloc(sizeof(dirEntry) * STARTING_NUM_DIR); 
    int blocks = (sizeof(dirEntry) * STARTING_NUM_DIR) / MBR_st -> blockSize + 1;
    LBAread(entryBuffer, blocks, currWorkingDirectory -> directoryStartLocation); 
    int freeIndex = -1; 
    for (int i = 0; i < STARTING_NUM_DIR; i++) {
        if ( entryBuffer[i] -> isBeingUsed == 0)
        {
           freeIndex = i;  
        }
    } 
    if (freeIndex == -1) {
        return -1
    }
    entryBuffer[freeIndex] -> isBeingUsed = 1; 
    strcpy(entryBuffer[freeIndex], pathname);
    LBAwrite(entryBuffer, blocks, currWorkingDirectory -> directoryStartLocation);
    free(entryBuffer); 
    return initDirectory(currWorkingDirectory -> directoryStartLocation); 

}
int fs_rmdir(const char *pathname){
    dirEntry *entryBuffer = (dirEntry *)malloc(sizeof(dirEntry) * STARTING_NUM_DIR); 
    int blocks = (sizeof(dirEntry) * STARTING_NUM_DIR) / MBR_st -> blockSize + 1;
    LBAread(entryBuffer, blocks, currWorkingDirectory -> directoryStartLocation); 
    int remove_index = -1
    for (int i = 0; i < STARTING_NUM_DIR; i++) {
        if (strcmp(entryBuffer[i] -> name, pathname) == 0)
        {
           remove_index = i;  
        }
    }
    fs_remove_helper(entryBuffer[remove_index] -> locationLBA);
    free(entryBuffer); 

}
int fs_remove_helper(int blockToRemove) {
    dirEntry *entryBuffer = (dirEntry *)malloc(sizeof(dirEntry) * STARTING_NUM_DIR); 
    int blocks = (sizeof(dirEntry) * STARTING_NUM_DIR) / MBR_st -> blockSize + 1;
    LBAread(entryBuffer, blocks, blockToRemove); 
    for (int i = 0; i < STARTING_NUM_DIR; i++) {
        if (entryBuffer[i] -> isFile == 1) {
            // deletefile();
            // free_mem(fileStartLBA, fileLength); 
        } else if (entryBuffer[i] -> isBeingUsed == 1) {
            fs_remove_helper(entryBuffer[i] -> locationLBA)
        }
    }
    free_mem(blockToRemove, blocks);
    entryBuffer[i] -> isBeingUsed = 0; 
    free(entryBuffer);  
}

